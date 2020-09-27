//////////////////////////////////////////////////////////////////////
//
// MonoWave.cpp : implementation file
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <mmsystem.h>
#include <mmreg.h>
#include <msacm.h>
#include "DLSDesignerDLL.h"
#include "WaveNode.h"
#include "Wave.h"
#include "WaveDataManager.h"
#include "StereoWave.h"
#include "MonoWave.h"
#include "Region.h"
#include "Collection.h"
#include "DLSLoadSaveUtils.h"
#include "WavePropPgMgr.h"
#include "WavePropPg.h"
#include "WaveInfoPropPg.h"
#include "WaveCompressionPropPage.h"
#include "stream.h"
#include "Instrument.h"
#include "Wavectl.h"
#include "DlsDefsPlus.h"
#include "CollectionInstruments.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
//
//	CMonoWaveDrawBuffer
//
//////////////////////////////////////////////////////////////////////
CMonoWaveDrawBuffer::CMonoWaveDrawBuffer(BYTE mwdbType):
	m_mwdbType(mwdbType),
	m_pMonoWave(NULL),
	m_pMonoWaveDrawBuffer(NULL),
	m_pbBuffer(NULL),
	m_nSamplesPerPixel(0),
	m_wBytesPerSample(0)
{}

CMonoWaveDrawBuffer::~CMonoWaveDrawBuffer()
{
	if (m_pbBuffer)
		delete[] m_pbBuffer;
}

/* data will be built from wave file */
void CMonoWaveDrawBuffer::Init(int nSamplesPerPixel, CMonoWave* pMonoWave)
{
	ASSERT((m_pMonoWave == NULL) && (m_pMonoWaveDrawBuffer == NULL)); // only one Init should be called
	ASSERT(pMonoWave);
	m_nSamplesPerPixel = nSamplesPerPixel;
	m_pMonoWave = pMonoWave;
}

/* data will be built from another zoom object */
void CMonoWaveDrawBuffer::Init(int nSamplesPerPixel, CMonoWaveDrawBuffer* pMonoWaveDrawBuffer)
{
	ASSERT((m_pMonoWave == NULL) && (m_pMonoWaveDrawBuffer == NULL)); // only one Init should be called
	ASSERT(pMonoWaveDrawBuffer);
	ASSERT(pMonoWaveDrawBuffer->GetSamplesPerPixel() < nSamplesPerPixel); // draw buffer doesn't have enough detail for us
	ASSERT(pMonoWaveDrawBuffer->GetType() == m_mwdbType);	// cannot compute from a buffer of a different type
	m_nSamplesPerPixel = nSamplesPerPixel;
	m_pMonoWaveDrawBuffer = pMonoWaveDrawBuffer;
}

/* ensures the buffer is filled from dwStartSample to a length of dwDrawLength. Returns
	pointer to buffer, and offsets to start and stop reading within the buffer. */
HRESULT	CMonoWaveDrawBuffer::Get(DWORD dwSampleStart, DWORD dwDrawLength,
	BYTE **ppbBuffer, DWORD *pdwBufferLength)
{
	ASSERT((m_pMonoWave != NULL) || (m_pMonoWaveDrawBuffer != NULL)); // Init must have been called
	ASSERT(ppbBuffer);
	ASSERT(pdwBufferLength);
	ASSERT(m_nSamplesPerPixel > 0);

	// set critical section
	HRESULT hr;
	CRITICAL_SECTION *pcs = GetCriticalSection();
	EnterCriticalSection(pcs);

	// get bytes per sample
	m_wBytesPerSample = GetBytesPerSample();

	// if empty wave, return immediately
	DWORD cSamplesInWave = GetWaveLength();
	if (cSamplesInWave == 0)
		{
		*ppbBuffer = NULL;
		*pdwBufferLength = 0;
		hr = S_OK;
		goto LExit;
		}
	
	// cap arguments
	if (dwSampleStart > cSamplesInWave)
		dwSampleStart = cSamplesInWave;
	if (dwSampleStart+dwDrawLength > cSamplesInWave)
		dwDrawLength = cSamplesInWave-dwSampleStart;

	// compute desired start and end positions
	DWORD dwUnitStart, dwUnitEnd, cUnits;
	dwUnitStart = dwSampleStart / m_nSamplesPerPixel;
	dwUnitEnd = (dwSampleStart + dwDrawLength) / m_nSamplesPerPixel;
	cUnits = dwUnitEnd - dwUnitStart;

	// destroy buffer now if it needs to be grown
	if ((m_pbBuffer != NULL) && (cUnits > m_cUnits))
		{
		delete[] m_pbBuffer;
		m_pbBuffer = NULL;
		}

	// build buffer if we don't have it yet
	if (m_pbBuffer == NULL)
		{
		// compute desired units, and narrow if possible to conserve memory
		m_cUnits = cSamplesInWave / m_nSamplesPerPixel;
		m_dwUnitOffset = 0;
		DWORD cUnitDesiredMax = max(2000, cSamplesInWave / 512); // our rule of thumb is to have the full buffer for 512 samples/pixel, with a minimum of a good screen's width
		if (cUnits > cUnitDesiredMax)
			cUnitDesiredMax = cUnits; // hold at least as many samples as required by caller
		if (m_cUnits > cUnitDesiredMax)
			{
			m_cUnits = cUnitDesiredMax;

			// narrowed buffer is centered on the region we're asked for
			DWORD dwUnitCenterDouble = dwUnitStart + dwUnitEnd;
			if (dwUnitCenterDouble > m_cUnits)
				m_dwUnitOffset = (dwUnitCenterDouble - m_cUnits) / 2;
			}

		// compute buffer size
		DWORD dwBufferSize = m_cUnits * m_wBytesPerSample;

		// allocate
		m_pbBuffer = new BYTE[dwBufferSize];
		if (m_pbBuffer == NULL)
			{
			hr = E_OUTOFMEMORY;
			goto LExit;
			}

		m_dwUnitStart = m_dwUnitEnd = m_dwUnitOffset;
		}

	ASSERT(m_cUnits > 0);

	// scroll or dirty buffer if necessary
	if (dwUnitStart < m_dwUnitOffset)
		{
		// scroll left
		DWORD dwDelta = m_dwUnitOffset - dwUnitStart;
		m_dwUnitOffset = dwUnitStart;
		if (dwDelta < m_cUnits)
			{
			memmove(&m_pbBuffer[dwDelta*m_wBytesPerSample], &m_pbBuffer[0], (m_cUnits-dwDelta)*m_wBytesPerSample);
			if (m_dwUnitEnd > m_dwUnitOffset+m_cUnits)
				{
				m_dwUnitEnd = m_dwUnitOffset+m_cUnits;
				if (m_dwUnitStart > m_dwUnitEnd)
					m_dwUnitStart = m_dwUnitEnd;
				}
			}
		else
			m_dwUnitStart = m_dwUnitEnd;
		}
	else if (dwUnitEnd > m_dwUnitOffset+m_cUnits)
		{
		// scroll right
		DWORD dwDelta = dwUnitEnd - (m_dwUnitOffset+m_cUnits);
		m_dwUnitOffset = dwUnitEnd - m_cUnits;
		if (dwDelta < m_cUnits)
			{
			memmove(&m_pbBuffer[0], &m_pbBuffer[dwDelta*m_wBytesPerSample], (m_cUnits-dwDelta)*m_wBytesPerSample);
			if (m_dwUnitStart < m_dwUnitOffset)
				{
				m_dwUnitStart = m_dwUnitOffset;
				if (m_dwUnitEnd < m_dwUnitStart)
					m_dwUnitEnd = m_dwUnitStart;
				}
			}
		else
			m_dwUnitStart = m_dwUnitEnd;
		}

	// fill in the data we haven't computed yet
	if (m_dwUnitStart == m_dwUnitEnd)
		{
		hr = Fill(dwUnitStart, dwUnitEnd);
		if (FAILED(hr))
			goto LExit;
		m_dwUnitStart = dwUnitStart;
		m_dwUnitEnd = dwUnitEnd;
		}
	else
		{
		if (dwUnitStart < m_dwUnitStart)
			{
			hr = Fill(dwUnitStart, m_dwUnitStart);
			if (FAILED(hr))
				goto LExit;
			m_dwUnitStart = dwUnitStart;
			}
		if (dwUnitEnd > m_dwUnitEnd)
			{
			hr = Fill(m_dwUnitEnd, dwUnitEnd);
			if (FAILED(hr))
				goto LExit;
			m_dwUnitEnd = dwUnitEnd;
			}
		}

	// we're done
	*ppbBuffer = &m_pbBuffer[(dwUnitStart-m_dwUnitOffset)*m_wBytesPerSample];
	*pdwBufferLength = dwUnitEnd-dwUnitStart;
	hr = S_OK;

LExit:
	LeaveCriticalSection(pcs);
	return hr;
}

/* fills the buffer from dwStart to dwEnd */
HRESULT CMonoWaveDrawBuffer::Fill(DWORD dwUnitStart, DWORD dwUnitEnd)
{
#ifdef _DEBUG
	ASSERT(m_pbBuffer);
	ASSERT(dwUnitStart < dwUnitEnd); // nothing to compute
	if (m_dwUnitStart != m_dwUnitEnd)
		{
		if (dwUnitStart < m_dwUnitStart)
			ASSERT(dwUnitEnd <= m_dwUnitStart); // overlap with already-computed data
		if (dwUnitEnd >= m_dwUnitEnd)
			ASSERT(dwUnitStart >= m_dwUnitEnd); // overlap with already-computed data
		}
#endif

	int nSampleStep = m_nSamplesPerPixel;
	DWORD dwSourceLength;

	// compute what data we need to get
	DWORD dwReadStart = dwUnitStart * m_nSamplesPerPixel;
	DWORD dwReadLength = (dwUnitEnd-dwUnitStart)*m_nSamplesPerPixel;
	
	BYTE* pbSource = NULL;
	if (m_pMonoWave)
		{
		// get the data from a wave

		// turn on wait cursor for slow reads
		CWaitCursor* pCursor = NULL;
		if (dwReadLength > 100000)
			pCursor = new CWaitCursor();
	
		if(FAILED(m_pMonoWave->GetChannelData(dwReadStart, dwReadLength, &pbSource)))
			return E_FAIL;
		dwSourceLength = dwReadLength;

		if (pCursor)
			delete pCursor;
		}
	else
		{
		// get the data from another buffer
		if (FAILED(m_pMonoWaveDrawBuffer->Get(dwReadStart, dwReadLength, &pbSource, &dwSourceLength)))
			return E_FAIL;
		nSampleStep /= m_pMonoWaveDrawBuffer->GetSamplesPerPixel();
		}

	// going to use dwUnitStart as an index in the buffer
	dwUnitStart -= m_dwUnitOffset;
	dwUnitEnd -= m_dwUnitOffset;

	switch (m_mwdbType)
		{
		case mwdbTypeWaveEditor:
			if (nSampleStep == 1)
				{
				// we'll draw every single sample....
				memcpy(&m_pbBuffer[dwUnitStart*m_wBytesPerSample], pbSource, dwSourceLength*m_wBytesPerSample);
				}
			else
				{
				// weed out the unwanted samples....
				if (m_wBytesPerSample == 1)
					{
					for(DWORD dwSourceSample = 0; dwSourceSample < dwSourceLength; dwSourceSample += nSampleStep)
						m_pbBuffer[dwUnitStart++] = pbSource[dwSourceSample];
					}
				else if (m_wBytesPerSample == 2)
					{
					short* pnBuffer = (short*)m_pbBuffer;
					short* pnSource = (short*)pbSource;

					for(DWORD dwSourceSample = 0; dwSourceSample < dwSourceLength; dwSourceSample += nSampleStep)
						pnBuffer[dwUnitStart++] = pnSource[dwSourceSample];
					}
				else
					{
					ASSERT(FALSE); // sample size not supported
					return E_FAIL;
					}
				}
			break;

		case mwdbTypeWaveTrack:
			{
			// search for the highest value in nSampleStep number of samples
			DWORD dwSamplesChecked = 0;
			if(m_wBytesPerSample == 2)
				{
				// Now add this sample to the draw buffer
				short* pnDrawBuffer = (short*)m_pbBuffer;
				short* pnWaveData = (short*)pbSource;

				for(DWORD dwUnit = dwUnitStart; dwUnit < dwUnitEnd; dwUnit++)
					{
					short nHighestSampleValue = 0;
					for(int nIndex = 0; nIndex < (int)nSampleStep; nIndex++)
						{
						short nSampleValue = pnWaveData[dwSamplesChecked + nIndex];
						if(abs(nSampleValue) > nHighestSampleValue)
							nHighestSampleValue = abs(nSampleValue);
						}
				
					*(pnDrawBuffer + dwUnit) = nHighestSampleValue;
					dwSamplesChecked += nSampleStep;
					}
				}
			else
				{
				ASSERT(m_wBytesPerSample == 1); // sample size not supported
				for(DWORD dwUnit = dwUnitStart; dwUnit < dwUnitEnd; dwUnit++)
					{
					int nHighestSampleValue = 0;
					for(int nIndex = 0; nIndex < (int)nSampleStep; nIndex++)
						{
						int nSampleValue = pbSource[dwSamplesChecked + nIndex];
						if(abs(nSampleValue) > nHighestSampleValue)
							nHighestSampleValue = abs(nSampleValue);
						}

					// Now add this sample to the draw buffer
					*(m_pbBuffer + dwUnit) = BYTE(nHighestSampleValue);
					dwSamplesChecked += nSampleStep;
					}
				}
			}
			break;

		default:
			ASSERT(FALSE); // wave buffer type not supported
		}

	// clean up buffer if we allocated it
	if (m_pMonoWave)	
		delete[] pbSource;

	return S_OK;
}

/* buffer is dirty and should be recomputed on next Fill call */
void CMonoWaveDrawBuffer::Dirty()
{
	// we don't deallocate our buffer, we simply mark its data as dirty
	m_dwUnitStart = m_dwUnitEnd = m_wBytesPerSample = 0;
}

/* returns the length of the referenced wave */
DWORD CMonoWaveDrawBuffer::GetWaveLength()
{
	ASSERT((m_pMonoWave != NULL) || (m_pMonoWaveDrawBuffer != NULL)); // Init must have been called
	return m_pMonoWave ?
		m_pMonoWave->GetWaveLength() :
		m_pMonoWaveDrawBuffer->GetWaveLength();
}

/* returns the number of bytes per sample */
WORD CMonoWaveDrawBuffer::GetBytesPerSample()
{
	ASSERT((m_pMonoWave != NULL) || (m_pMonoWaveDrawBuffer != NULL)); // Init must have been called
	if (m_wBytesPerSample == 0)
		m_wBytesPerSample = m_pMonoWave ?
			m_pMonoWave->m_rWaveformat.wBitsPerSample / 8 :
			m_pMonoWaveDrawBuffer->GetBytesPerSample();
	return m_wBytesPerSample;
}

/* returns the wave's critical section */
CRITICAL_SECTION* CMonoWaveDrawBuffer::GetCriticalSection()
{
	return m_pMonoWave ?
		&m_pMonoWave->m_DrawBufferCreateLock :
		m_pMonoWaveDrawBuffer->GetCriticalSection();
}


//////////////////////////////////////////////////////////////////////
//
//	CMonoWave::CMonoWave
//
//////////////////////////////////////////////////////////////////////
CMonoWave::CMonoWave(CWaveNode* pNode, DWORD dwSampleRate, UINT nSampleSize) : CWave(pNode, dwSampleRate, nSampleSize, 1), 
m_dwDLSize(0),	
m_pdbWaveTrack(NULL),
m_bStripChannelData(false),
m_bRightChannel(false),
m_pdbOptimum(NULL),
m_pdbLatest(NULL),
m_pdb(NULL),
m_nSamplesPerPixelOptimum(0)
{
	ASSERT( pNode != NULL );

    m_pnWave = NULL;

 	m_dwDataSize = 0;
    m_fReadFormat = FALSE;
    m_fReadSMPL = FALSE;
    m_fReadWSMP = FALSE;

	if(m_pDataManager == NULL)
	{
		CString sSourceFileName = m_pWaveNode->GetFileName();
		char szTempPath[MAX_PATH];
		DWORD dwSuccess = GetTempPath(MAX_PATH, szTempPath);
		m_pDataManager = new CWaveDataManager(this, sSourceFileName, szTempPath);
	}

    InitializeCriticalSection(&m_DrawBufferCreateLock);

}

CMonoWave::CMonoWave(CWaveNode* pNode, GUID guidStereoWave, bool bRightChannel, DWORD dwSampleRate, UINT nSampleSize) : CWave(pNode, dwSampleRate, nSampleSize, 1),
m_guidStereoWave(guidStereoWave), 
m_bRightChannel(bRightChannel),
m_pdbWaveTrack(NULL),
m_dwDLSize(0),	
m_bStripChannelData(false),
m_pdbOptimum(NULL),
m_pdbLatest(NULL),
m_pdb(NULL),
m_nSamplesPerPixelOptimum(0)
{
	ASSERT( pNode != NULL );

    m_pnWave = NULL;

 	m_dwDataSize = 0;
    m_fReadFormat = FALSE;
    m_fReadSMPL = FALSE;
    m_fReadWSMP = FALSE;

	if(m_pDataManager == NULL)
	{
		CString sSourceFileName = m_pWaveNode->GetFileName();
		char szTempPath[MAX_PATH];
		DWORD dwSuccess = GetTempPath(MAX_PATH, szTempPath);
		m_pDataManager = new CWaveDataManager(this, sSourceFileName, szTempPath);
	}

    InitializeCriticalSection(&m_DrawBufferCreateLock);
}


//////////////////////////////////////////////////////////////////////
//
//	CMonoWave::~CMonoWave()
//
//////////////////////////////////////////////////////////////////////
CMonoWave::~CMonoWave()
{
	ASSERT(m_pWaveNode);
	CDLSComponent* pComponent = m_pWaveNode->GetComponent();
	ASSERT(pComponent);

	if (!m_lstAllocatedBuffers.IsEmpty())
	{
		POSITION position = pComponent->m_lstDLSPortDownload.GetHeadPosition();
		while(position)
		{
			IDirectMusicPortDownload* pIDMPortDownload = pComponent->m_lstDLSPortDownload.GetNext(position);
			ASSERT(pIDMPortDownload);
			Unload(pIDMPortDownload);
		}
	}

	if (m_pdbWaveTrack)
		delete m_pdbWaveTrack;

	if (m_pdbLatest)
		delete m_pdbLatest;
	if (m_pdbOptimum)
		delete m_pdbOptimum;

    DeleteCriticalSection(&m_DrawBufferCreateLock);
}


//////////////////////////////////////////////////////////////////////
//
//	CMonoWave::Save
//
//////////////////////////////////////////////////////////////////////
HRESULT CMonoWave::Save(IDMUSProdRIFFStream* pIRiffStream, UINT wType, BOOL fFullSave)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT(m_pDataManager);
    if(m_pDataManager == NULL)
    {
        return E_UNEXPECTED;
    }

    // Ask the data mnager to update the uncomrpessed deltas
    if(!m_bCompressed)
    {
        if(FAILED(m_pDataManager->UpdateUncompressedDeltas()))
        {
            return E_FAIL;
        }
    }


	bool bRuntime = false;

	ASSERT(pIRiffStream);
	if(pIRiffStream == NULL)
	{
		return E_POINTER;
	}
	
	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_OUTOFMEMORY;
	}

	ASSERT(m_pDataManager);
	if(m_pDataManager == NULL)
	{
		return E_FAIL;
	}

	HRESULT			hr = S_OK;			
	MMCKINFO		ckMain;
	MMCKINFO		ck;
	DWORD			cb;
    DWORD           dwSaveSize;

    m_dwHeaderOffset = GetFilePos(pIStream);

    if(wType == MMIO_CREATERIFF)
    {
	    ckMain.fccType = mmioFOURCC('W','A','V','E');
    }
    else
    {
	    ckMain.fccType = FOURCC_wave;
    }
	hr = pIRiffStream->CreateChunk(&ckMain, wType);
    if(FAILED(hr))
	{
		pIStream->Release();
        return (hr);
    }

	IDMUSProdPersistInfo* pIJazzPersistInfo;
	if(SUCCEEDED(pIStream->QueryInterface(IID_IDMUSProdPersistInfo,(void**) &pIJazzPersistInfo)))
	{
		DMUSProdStreamInfo	StreamInfo;
		FileType	ftSaveType;

		pIJazzPersistInfo->GetStreamInfo(&StreamInfo);
		ftSaveType = StreamInfo.ftFileType;

		if (ftSaveType == FT_RUNTIME)
			bRuntime = true;
		
		pIJazzPersistInfo->Release();		
	}


	if(fFullSave)
	{
		
		// Write DMUS_FOURCC_GUID_CHUNK
		if(FAILED(CWave::WriteGUIDChunk(pIRiffStream)))
		{
			pIStream->Release();
			return E_FAIL; 
		}

		// Save the stereo wave information
		if(IsStereo())
		{
			ck.ckid = mmioFOURCC('w','v','s','t');
			hr = pIRiffStream->CreateChunk(&ck, 0);

			if (FAILED(hr))
			{
				pIStream->Release();
				return (hr);
			}

			// Save the flag that marks whether this is the right channel of a stereo wave
			hr = pIStream->Write((LPSTR) &(m_bRightChannel), sizeof(bool), &cb);	

			if(FAILED(hr) || cb != sizeof(bool))
			{
				pIStream->Release();
				return E_FAIL;
			}

			// Save the GUID that identifies this wave to the stereo wave
			hr = pIStream->Write((LPSTR)&(m_guidStereoWave), sizeof(GUID), &cb);
			if(FAILED(hr) || cb != sizeof(GUID))
			{
				pIStream->Release();
				return E_FAIL;
			}

			if (FAILED(pIRiffStream->Ascend(&ck, 0)))
			{
				pIStream->Release();
				return E_FAIL;
			}
		}

		
		// Compressed wave info chunk
		if(FAILED(WriteCompressionChunk(pIRiffStream, bRuntime)))
		{
			pIStream->Release();
			return E_FAIL;
		}

		// Write the 'fmt ' chunk
		if(FAILED(WriteFormat(pIRiffStream, bRuntime)))
		{
			pIStream->Release();
			return E_FAIL;
		}

		// Write the <fact-ck> if the wave is non-PCM
		if(m_bCompressed && bRuntime)
		{
			if(FAILED(WriteFACTChunk(pIRiffStream)))
			{
				pIStream->Release();
				return E_FAIL;
			}
		}
	}

	// Save the 'wavh' chunk that keeps the streaming info
	ck.ckid = DMUS_FOURCC_WAVEHEADER_CHUNK;
	hr = pIRiffStream->CreateChunk(&ck, 0);
    if (FAILED(hr))
    {
		pIStream->Release();
        return (hr);
    }

	_DMUS_IO_WAVE_HEADER waveHeader;
	if(FAILED(CWave::GetStreamingWaveHeader(&waveHeader)))
	{
		pIStream->Release();
		return E_FAIL;
	}

	hr = pIStream->Write((LPSTR)&waveHeader, sizeof(_DMUS_IO_WAVE_HEADER), &cb);
	if(FAILED(hr) || cb != sizeof(_DMUS_IO_WAVE_HEADER))
	{
		pIStream->Release();
		return E_FAIL;
	}
	
	if (FAILED(pIRiffStream->Ascend(&ck, 0)))
	{
		pIStream->Release();
		return E_FAIL;
	}


	// Save the RSMPL Info
	if(FAILED(WriteRSMPLChunk(pIRiffStream)))
	{
		pIStream->Release();
		return E_FAIL;
	}

	// Save the WSMPL Info
    ck.ckid = FOURCC_WSMP;
    hr = pIRiffStream->CreateChunk(&ck, 0);
    if (FAILED(hr))
    {
		pIStream->Release();
        return hr;
    }
    dwSaveSize = sizeof(WSMPL);
    if (m_rWSMP.cSampleLoops > 0)
    {
        dwSaveSize += sizeof(WLOOP);
        m_rWSMP.cSampleLoops = 1;
    }
    hr = pIStream->Write((LPSTR) &m_rWSMP,dwSaveSize, &cb);
    if (FAILED(pIRiffStream->Ascend(&ck, 0)) || cb != dwSaveSize)
    {
		pIStream->Release();
        return E_FAIL;
    }

	if (fFullSave)
	{
		// We position the data offset just after the chunk header
		m_dwDataOffset = GetFilePos(pIStream) + 8;

		ck.ckid = mmioFOURCC('d','a','t','a') ;
		hr = pIRiffStream->CreateChunk(&ck,0);
		if (FAILED(hr))
		{
			pIStream->Release();
			return hr;
		}

		if(bRuntime && m_bCompressed) 
		{
			UINT nExtraBytes = m_CompressedWavObj.WaveFormatEx.cbSize;
			WAVEFORMATEX* pwfxDest = NULL;
			if(FAILED(SafeAllocBuffer((BYTE**)&pwfxDest, sizeof(WAVEFORMATEX) + nExtraBytes)))
			{
				return E_OUTOFMEMORY;
			}

			CopyMemory((BYTE*)pwfxDest, &m_CompressedWavObj.WaveFormatEx, sizeof(WAVEFORMATEX));
			CopyMemory((BYTE*)pwfxDest + sizeof(WAVEFORMATEX), m_CompressedWavObj.pbExtractWaveFormatData, nExtraBytes); 

			// Save the compressed wave data
			hr = WriteCompressedData(pIStream, pwfxDest, cb);
			dwSaveSize = cb;

			delete[] pwfxDest;

		}
		else
		{
			if(m_pWaveNode->IsInACollection())
			{
				BYTE* pbData = NULL;
				if(FAILED(GetAllUncompressedChannelData(&pbData, &dwSaveSize)))
				{
					return E_FAIL;
				}
				
				if(FAILED(pIStream->Write(pbData, dwSaveSize, &cb)))
				{
					delete[] pbData;
					return E_FAIL;
				}

				delete[] pbData;
			}
			else
			{
                // This method writes down the uncompressed data and handles failures
                // So it's safe to set dwSaveSize to the bytes actually writeen down
				hr = m_pDataManager->SaveUncompressedDataToStream(pIStream, cb);
                dwSaveSize = cb;
			}
		}

		if (FAILED(hr) || cb != dwSaveSize)
		{
			pIRiffStream->Ascend(&ck, 0);
			pIStream->Release();
			return E_FAIL;
		}

		pIRiffStream->Ascend(&ck, 0);

        // Save unknown chunks
        m_UnknownChunk.Save(pIRiffStream);
		m_Info.Save(pIRiffStream);
	}

    pIRiffStream->Ascend(&ckMain, 0);
	pIStream->Release();

    return (hr);
}


HRESULT CMonoWave::ReadWVST(IStream* pIStream, MMCKINFO* pckMain)		   
{
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_POINTER;
	}
	
	ASSERT(pckMain);
	if(pckMain == NULL)
	{
		return E_POINTER;
	}

	DWORD cb = 0;
	HRESULT hr = pIStream->Read(&m_bRightChannel, sizeof(bool), &cb);
	if(FAILED(hr) || cb != sizeof(bool))
	{
		return E_FAIL;
	}
	
	hr = pIStream->Read(&m_guidStereoWave, sizeof(GUID), &cb);
	if(FAILED(hr) || cb != sizeof(GUID))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonoWave::ReadFMT(IStream* pIStream, MMCKINFO* pckMain)
{
	if(FAILED(CWave::ReadFMT(pIStream, pckMain)))
	{
		return E_FAIL;
	}

	WAVEFORMATEX* pwfxRead = &m_rWaveformat;
	if(m_bCompressed && m_bLoadingRuntime)
	{
		pwfxRead = &m_CompressedWavObj.WaveFormatEx;
	}

	// Must be a mono wave...
	if(pwfxRead->nChannels != 1 && IsStereo() == false)   // we only support mono and stereo files.
    {
        AfxMessageBox(IDS_WAVE_STEREO, MB_ICONEXCLAMATION); 
		return E_FAIL;
	}
	else if(IsStereo())
	{
		// Massage the data for the mono channel format
		pwfxRead->nChannels = 1;
		if(((pwfxRead->wBitsPerSample == 8) && (pwfxRead->nBlockAlign == 2)) ||
		  ((pwfxRead->wBitsPerSample == 16) && (pwfxRead->nBlockAlign == 4)))
		{
			pwfxRead->nBlockAlign = pwfxRead->nBlockAlign / 2;
		}
	}

	return S_OK;
}


HRESULT CMonoWave::ReadDataInfo(IStream* pIStream, MMCKINFO* pckMain)
{
	if(FAILED(CWave::ReadDataInfo(pIStream, pckMain)))
	{
		return E_FAIL;
	}

	if(IsStereo())
	{
		CStereoWave* pStereoWave = (CStereoWave*)m_pWaveNode->GetWave();
		ASSERT(pStereoWave);
		if(pStereoWave->IsTrueStereo())
		{
			m_dwDataSize /= 2;
			m_OriginalWavObj.dwDataSize = m_dwDataSize;
		}
	}

	// Compute wavelength
	m_dwWaveLength = m_dwDataSize;
	if(m_rWaveformat.wBitsPerSample == 16)
	{
		m_dwWaveLength = m_dwDataSize >> 1;
	}

	return S_OK;
}




//////////////////////////////////////////////////////////////////////
//
//	CMonoWave::Load
//
//////////////////////////////////////////////////////////////////////

HRESULT CMonoWave::Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain)
{
	ASSERT(m_pWaveNode);
	if(m_pWaveNode == NULL)
	{
		return E_UNEXPECTED;
	}

	if(FAILED(CWave::Load(pIRiffStream, pckMain)))
	{
		return E_FAIL;
	}

	return S_OK;
}


HRESULT CMonoWave::LoadHeader(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain)
{
	ASSERT(pIRiffStream);
	if(pIRiffStream == NULL)
	{
		return E_POINTER;
	}

	ASSERT(pckMain);
	if(pckMain == NULL)
	{
		return E_POINTER;
	}

	if(FAILED(CWave::LoadHeader(pIRiffStream, pckMain)))
	{
		return E_FAIL;
	}

	// Massage the data for the mono wave
	if(IsStereo())
	{
		// Massage the data for the mono channel format
		m_OriginalWavObj.WaveFormatEx.nChannels = 1;
		if(((m_OriginalWavObj.WaveFormatEx.wBitsPerSample == 8) && (m_OriginalWavObj.WaveFormatEx.nBlockAlign == 2)) ||
		  ((m_OriginalWavObj.WaveFormatEx.wBitsPerSample == 16) && (m_OriginalWavObj.WaveFormatEx.nBlockAlign == 4)))
		{
			m_OriginalWavObj.WaveFormatEx.nBlockAlign = m_OriginalWavObj.WaveFormatEx.nBlockAlign / 2;
            m_OriginalWavObj.WaveFormatEx.nAvgBytesPerSec = m_OriginalWavObj.WaveFormatEx.nSamplesPerSec * m_OriginalWavObj.WaveFormatEx.nBlockAlign;
		}
	}

	// Update the header stream
	// Stero waves will do their own update
	if(IsStereo() == FALSE)
	{
		if(SUCCEEDED(UpdateHeaderStream()))
		{
			m_pWaveNode->ReloadDirectSoundWave();
			NotifyWaveChange(false);
		}
	}

	return S_OK;
}


HRESULT CMonoWave::LoadData(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain)
{
	ASSERT(pIRiffStream);
	if(pIRiffStream == NULL)
	{
		return E_POINTER;
	}

	ASSERT(pckMain);
	if(pckMain == NULL)
	{
		return E_POINTER;
	}

	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = E_FAIL;
	MMCKINFO ck;
	ck.ckid = FOURCC_DATA;

	if(pIRiffStream->Descend(&ck, pckMain, MMIO_FINDCHUNK) == 0)
	{
		if(m_bCompressed && m_bLoadingRuntime)
		{
			hr = LoadRuntimeCompressedWave(pIStream, ck.cksize);
		}
		else
		{
			hr = LoadUncompressedOrDesignTimeWave(pIStream, ck.cksize);
		}
	}

	pIStream->Release();
	return hr;
}

HRESULT CMonoWave::LoadUncompressedOrDesignTimeWave(IStream* pIStream, DWORD dwChunkSize)
{
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_POINTER;
	}

	DWORD cbRead = 0;
	HRESULT hr = S_OK;
	BYTE* pbTempDataBuffer = NULL;
	CWaveDataManager* pDataManager = NULL;

	DWORD dwDataSize = dwChunkSize;
	if(IsStereo() && m_bStripChannelData)
	{
		dwDataSize = dwDataSize / 2;
	}

	// Allocate a buffer for the wave / left channel data
	if(FAILED(hr = SafeAllocBuffer(&m_OriginalWavObj.pbData, dwDataSize)))
	{
		goto ON_ERROR;
	}

	if(FAILED(hr = SafeAllocBuffer(&pbTempDataBuffer, dwChunkSize)))
	{
		goto ON_ERROR;
	}

	/*****************************************************/
	/* TEST CALL!!! Remove the block when checking in....*/
	/*****************************************************/
	pDataManager = GetDataManager();
	ASSERT(pDataManager);

	if(FAILED(pDataManager->GetData(0, m_dwWaveLength, pbTempDataBuffer, cbRead)))
	{
		goto ON_ERROR;
	}
	/*****************************************************/

	// Compute wavelength
	if (m_rWaveformat.wBitsPerSample == 8)
	{
		m_dwWaveLength = dwDataSize;
	}
	else
	{
		m_dwWaveLength = dwDataSize >> 1;
	}


	if(m_bStripChannelData)
	{
		if(FAILED(StripChannelData(pbTempDataBuffer, dwChunkSize, m_OriginalWavObj.pbData)))
		{
			goto ON_ERROR;
		}
	}
	else
	{
		CopyMemory(m_OriginalWavObj.pbData, pbTempDataBuffer, dwDataSize);
	}

	m_dwDataSize = dwDataSize;
	m_OriginalWavObj.dwDataSize = m_dwDataSize;

	// This is what is used to show the wav
	m_pnWave = (short*)m_OriginalWavObj.pbData;

	// This might be a designtime file that has waves marked compressed 
	// So we need to update the compressed wave object
	if(m_bCompressed)
	{
		if(FAILED(CompressLoadedWaveData()))
		{
			// On failure to compress the original data to the required compressed format
			// we use the original with compression turned off
			// The user will now have to use different compression format and recompress the wave
			// Turn the compression flag off
			m_bCompressed = false;
		}
		
	}

	// Delete the temporary buffer
	if(pbTempDataBuffer)
	{
		delete[] pbTempDataBuffer;
	}

	return hr;

	// Cleanup on error
ON_ERROR:
	if(pIStream)
	{
		pIStream->Release();
	}
	pIStream->Release();
	CleanWaveObject(&m_OriginalWavObj);
	CleanWaveObject(&m_CompressedWavObj);
	CleanWaveObject(&m_DecompressedWavObj);
	if(pbTempDataBuffer)
	{
		delete[] pbTempDataBuffer;
		pbTempDataBuffer = NULL;
	}

	return hr;
}

// This method assumes that the compression format has been successfully read into
// the compressed wave object....
HRESULT CMonoWave::LoadRuntimeCompressedWave(IStream* pIStream, DWORD dwChunkSize)
{
	// No stream?
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_POINTER;
	}

	// Allocate a fresh buffer for the compressed data
	m_CompressedWavObj.pbData = new BYTE[dwChunkSize];
	if(m_CompressedWavObj.pbData == NULL)
	{
		return E_OUTOFMEMORY;
	}

	// Keeps track of number of bytes read from the stream for each read attempt
	DWORD cbRead = 0;

	// Read in the data from the stream
	ZeroMemory(m_CompressedWavObj.pbData, dwChunkSize);
	HRESULT hr = pIStream->Read(m_CompressedWavObj.pbData, dwChunkSize, &cbRead);
	if(FAILED(hr) || cbRead != dwChunkSize)
	{
		AfxMessageBox(IDS_ERR_RUNTIME_UNSUPPORTED_COMPRESSION_FORMAT);
		return E_FAIL;
	}

	// Set the size of the compressed data in the wave object
	m_CompressedWavObj.dwDataSize = dwChunkSize;

	// Check if the compression format is supported 
	if(!m_CompressionManager.GetSupportedFormats(m_CompressedWavObj.WaveFormatEx.wFormatTag, this))
	{
		// Unknown format
		AfxMessageBox(IDS_ERR_RUNTIME_UNSUPPORTED_COMPRESSION_FORMAT);
		return E_FAIL; 
	}

	// Compressed wave formats may have extra information after the standard WAVEFORMATEX
	DWORD dwExtraBytes = m_CompressedWavObj.WaveFormatEx.cbSize;

	// Decompresses the data to the passed PCM format
	if(SUCCEEDED(UpdateDecompressedWave(&m_rWaveformat)))
	{
		// Clear up the old data
		CleanWaveObject(&m_OriginalWavObj);
		CopyWaveObject(&m_DecompressedWavObj, &m_OriginalWavObj);
		m_dwDataSize = m_DecompressedWavObj.dwDataSize;
	}
	else
	{
		AfxMessageBox(IDS_ERR_RUNTIME_UNSUPPORTED_COMPRESSION_FORMAT);
		return hr;
	}

	// Success!
	return S_OK;
}


// This method assumes that the original wave data has been successfuly read
HRESULT CMonoWave::CompressLoadedWaveData()
{
	// Check if the compression format is supported 
	if(!m_CompressionManager.GetSupportedFormats(m_CompressedWavObj.WaveFormatEx.wFormatTag, this))
	{
		AfxMessageBox(IDS_ERR_UNSUPPORTED_COMPRESSION_FORMAT);
		return E_FAIL;
	}

	// Non-PCM formats have extra information after standard WAVEFORMATEX struct
	DWORD dwExtraBytes = m_CompressedWavObj.WaveFormatEx.cbSize;

	// Copy the compression format
	WAVEFORMATEX* pCompressionFormat = (WAVEFORMATEX* )new BYTE[sizeof(WAVEFORMATEX) + dwExtraBytes];
	if(pCompressionFormat == NULL)
	{
		return E_OUTOFMEMORY;
	}

	ZeroMemory(pCompressionFormat, sizeof(WAVEFORMATEX) + dwExtraBytes);
	CopyMemory(pCompressionFormat, &(m_CompressedWavObj.WaveFormatEx), sizeof(WAVEFORMATEX));
	CopyMemory((char*)(pCompressionFormat) + sizeof(WAVEFORMATEX), m_CompressedWavObj.pbExtractWaveFormatData, dwExtraBytes);

	// Find out and adjust the sample loss
	DWORD dwDifference = 0;
	if(FAILED(FindSampleLoss(pCompressionFormat, dwDifference)))
	{
		if(pCompressionFormat->nChannels != 2 && m_pCollection != NULL)
		{
			AfxMessageBox(IDS_ERR_UNSUPPORTED_COMPRESSION_FORMAT);
		}

		if(pCompressionFormat)
		{
			delete[] pCompressionFormat;
		}
		
		return E_FAIL;
	}

	// Clean the wave objects (we might be here through the Undo/Redo action
	CleanWaveObject(&m_CompressedWavObj);
	CleanWaveObject(&m_DecompressedWavObj);

	// Compress the wave data to the required format
	HRESULT hr = ConvertWave(m_OriginalWavObj, pCompressionFormat, m_CompressedWavObj, true, dwDifference);
	if(SUCCEEDED(hr))
	{
		// Update the decompressed data
		hr = UpdateDecompressedWave(&(m_OriginalWavObj.WaveFormatEx));
	}
	
	// Get rid of the compression format buffer
	if(pCompressionFormat)
	{
		delete[] pCompressionFormat;
	}

	if(FAILED(hr))
	{
		AfxMessageBox(IDS_ERR_UNSUPPORTED_COMPRESSION_FORMAT);
	}

	return hr;
}


//////////////////////////////////////////////////////////////////////
//
//	CMonoWave::UpdateWave()
//
//////////////////////////////////////////////////////////////////////
HRESULT CMonoWave::UpdateWave()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	HRESULT hr = S_OK;
	ASSERT(m_pWaveNode);

	CDLSComponent* pComponent = m_pWaveNode->GetComponent();
	ASSERT(pComponent);
	
	// Return if there' no valid port
	if(pComponent->m_lstDLSPortDownload.IsEmpty())
		return E_FAIL;

	POSITION position = pComponent->m_lstDLSPortDownload.GetHeadPosition();
	while(position)
	{
		IDirectMusicPortDownload* pIDMPortDownload = pComponent->m_lstDLSPortDownload.GetNext(position);		
		ASSERT(pIDMPortDownload);

        // Remember the download Id for the old port, if any
        DWORD dwDMID;
        bool fGotID = SUCCEEDED(GetDMIDForPort(pIDMPortDownload, &dwDMID));

		// unload the wave if necessary
		Unload(pIDMPortDownload);

		// nothing to download if new wave is empty
		if (m_dwWaveLength == 0)
			return S_OK;

		// initialize port, reusing the id
		if (fGotID)
			hr = DM_Init(pIDMPortDownload, dwDMID); // reuse the download id
		else
			hr = DM_Init(pIDMPortDownload); // get a new download id

		// download
		if (SUCCEEDED(hr))
			{
			// download, with wait cursor if necessary
			CWaitCursor *pcursor = (m_dwDLSize > 100000) ? new CWaitCursor() : NULL;
			hr = Download(pIDMPortDownload);
			if (pcursor)
				delete pcursor;

			if (!SUCCEEDED(hr))
				{
				// Remove the added buffer
				AllocatedPortBufferPair* pBuffer = GetAllocatedBufferForPort(pIDMPortDownload);
				ASSERT(pBuffer); // buffer should exist
				if (pBuffer)
					RemoveFromAllocatedBufferList(pBuffer);
	
				AfxMessageBox(IDS_ERR_UPDATE_DOWNLOAD, MB_OK | MB_ICONEXCLAMATION);
				}
			}
		}

	return hr;
} 

CString CMonoWave::GetName()
{
	if(IsStereo())
	{
		if(!m_bRightChannel)
			return m_Info.m_csName + "(Left)";
		else
			return m_Info.m_csName + "(Right)";
	}

	return m_Info.m_csName;
}

HRESULT CMonoWave::Size(DWORD& dwSize)
{
	HRESULT hr = S_OK;
	
	DWORD dwTotalSize = 0;
	DWORD dwOffsetCount = 0;
	
	dwSize = 0;
	__try
	{
		// Calculate the space needed for DMUS_DOWNLOADINFO 
		dwSize += CHUNK_ALIGN(sizeof(DMUS_DOWNLOADINFO));

		// Calculate the space needed for wave's copyright
		if(!m_Info.m_csCopyright.IsEmpty())
		{
			dwSize += CHUNK_ALIGN(sizeof(DMUS_COPYRIGHT));		
			
			// Add Length + 1 for null char
			dwSize += m_Info.m_csCopyright.GetLength() + 1;	
			dwOffsetCount++;
		}
		// If wave does not have one use collection's
		else if( (m_pCollection) && (!m_pCollection->m_Info.m_csCopyright.IsEmpty()) )
		{
			dwSize += CHUNK_ALIGN(sizeof(DMUS_COPYRIGHT));						
			
			// Add Length + 1 for null char
			dwSize += m_pCollection->m_Info.m_csCopyright.GetLength() + 1; 
			dwOffsetCount++;
		}
		
		// Calculate space needed for DMUS_WAVE and DMUS_WAVEDATA
		dwSize += CHUNK_ALIGN(sizeof(DMUS_WAVE)) + CHUNK_ALIGN(sizeof(DMUS_WAVEDATA));
		dwOffsetCount += 2;

		// Extra bytes that follow the WAVEFOMATEX structure
#if(0)	// For now we won't support until all compression support is implemented
		if ( m_rWaveformat.wFormatTag != WAVE_FORMAT_PCM )
		{
			//some other test?
			dwSize += m_rWaveformat.cbSize;
			dwOffsetCount++;
		}
#endif			

		// Calculate space needed for offset table
		m_cbSizeOffsetTable = CHUNK_ALIGN(dwOffsetCount * sizeof(ULONG));
		dwSize += m_cbSizeOffsetTable;

		// Compute leading data we'll skip
		DWORD cbDecompressedStart = m_bCompressed ? m_dwDecompressedStart*(m_rWaveformat.wBitsPerSample/8) : 0;

		// Calculate space needed for wave data
		// We allocated DMUS_MIN_DATA_SIZE worth when we allocated for DMUS_WAVE
		if(m_dwDataSize > DMUS_MIN_DATA_SIZE+cbDecompressedStart)
		{
			dwSize += m_dwDataSize - DMUS_MIN_DATA_SIZE - cbDecompressedStart;
		}

		dwSize = CHUNK_ALIGN(dwSize);

	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		hr = E_UNEXPECTED;
	}
	
	return hr;
}

HRESULT CMonoWave::Write(void * pvoid, IDirectMusicPortDownload* pIDMPortDownLoad)
{
	ASSERT(pvoid);
    if(pvoid == NULL)
    {
        return E_POINTER;
    }

    ASSERT(pIDMPortDownLoad);
    if(pIDMPortDownLoad == NULL)
    {
        return E_POINTER;
    }

	Size(m_dwDLSize);

	HRESULT hr = S_OK;

	__try
	{
		DWORD dwCurIndex = 0;	// Used to determine what index to store offset in Offset Table
		DWORD dwCurOffset = 0;	// Offset relative to beginning of passed in memory
		// Write DMUS_DOWNLOADINFO		
		DMUS_DOWNLOADINFO *pDLInfo = (DMUS_DOWNLOADINFO *) pvoid;

		pDLInfo->dwDLType = DMUS_DOWNLOADINFO_WAVE;
		DWORD dwDMid;
        if(FAILED(GetDMIDForPort(pIDMPortDownLoad, &dwDMid)))
        {
            return E_FAIL;
        }
        pDLInfo->dwDLId = dwDMid;

		pDLInfo->dwNumOffsetTableEntries = (m_cbSizeOffsetTable/sizeof(DWORD));
		pDLInfo->cbSize = m_dwDLSize;
		
		dwCurOffset += CHUNK_ALIGN(sizeof(DMUS_DOWNLOADINFO));

		DMUS_OFFSETTABLE* pDMOffsetTable = (DMUS_OFFSETTABLE *)(((BYTE*)pvoid) + dwCurOffset);

		// Increment pass the DMUS_OFFSETTABLE structure; we will fill the other members in later
		dwCurOffset += CHUNK_ALIGN(m_cbSizeOffsetTable);

		// First entry in ulOffsetTable is the first data chunk - the DMUS_WAVE in this case.
		pDMOffsetTable->ulOffsetTable[0] = dwCurOffset;

		dwCurIndex = 2;		// First two items are DMUS_WAVE and DMUS_WAVEDATA;

		DMUS_WAVE* pDMWave = (DMUS_WAVE*)(((BYTE*)pvoid) + dwCurOffset);
		pDMWave->ulWaveDataIdx = 1;		// Point to the DMUS_WAVEDATA chunk.

		dwCurOffset += CHUNK_ALIGN(sizeof(DMUS_WAVE));

		// download wave data
		DMUS_WAVEDATA* pDMWaveData = (DMUS_WAVEDATA*)(((BYTE*)pvoid) + dwCurOffset);
		pDMOffsetTable->ulOffsetTable[1] = dwCurOffset;
		BYTE* pbData = NULL;
		DWORD dwDecompressedStart = m_bCompressed ? m_dwDecompressedStart : 0;
		if(FAILED(GetChannelData(dwDecompressedStart, m_dwWaveLength-dwDecompressedStart, &pbData)))
			return E_FAIL;
		DWORD cbDecompressedStart = dwDecompressedStart*(m_rWaveformat.wBitsPerSample/8);
		ASSERT(m_dwDataSize >= cbDecompressedStart);
		DWORD dwDataSize = m_dwDataSize-cbDecompressedStart;
		CopyMemory(pDMWaveData->byData, pbData, dwDataSize);
		pDMWaveData->cbSize = dwDataSize;
		delete[] pbData;

		if(dwDataSize < DMUS_MIN_DATA_SIZE)
			dwCurOffset += sizeof(DMUS_WAVEDATA);
		else
			dwCurOffset += sizeof(DMUS_WAVEDATA) + dwDataSize - DMUS_MIN_DATA_SIZE;
		dwCurOffset = CHUNK_ALIGN(dwCurOffset);

		CopyMemory(&(pDMWave->WaveformatEx), &m_rWaveformat, sizeof(WAVEFORMATEX));

		// Write no extension chunks
		pDMWave->ulFirstExtCkIdx = 0;
		
		DWORD	dwStrSize=0;
		// Write copyright information
		if(!m_Info.m_csCopyright.IsEmpty())
		{
			dwStrSize = m_Info.m_csCopyright.GetLength() + 1;
			pDMOffsetTable->ulOffsetTable[dwCurIndex] = dwCurOffset;
			pDMWave->ulCopyrightIdx = dwCurIndex;
			// Copy the cbSize value
			CopyMemory((BYTE*)pvoid + dwCurOffset, &dwStrSize, sizeof(ULONG));

			// Copy the actual Copyright String
			dwCurOffset += sizeof(ULONG);
			CopyMemory((BYTE*)pvoid + dwCurOffset,m_Info.m_csCopyright.GetBuffer(0),dwStrSize);

			dwCurIndex++;
			m_Info.m_csCopyright.ReleaseBuffer();
		}
		// If instrument does not have one use collection's
		else if((m_pCollection) && (!m_pCollection->m_Info.m_csCopyright.IsEmpty()))
		{
			dwStrSize = m_pCollection->m_Info.m_csCopyright.GetLength() + 1;
			pDMOffsetTable->ulOffsetTable[dwCurIndex] = dwCurOffset;
			pDMWave->ulCopyrightIdx = dwCurIndex;

			// Copy the cbSize value
			CopyMemory((BYTE*)pvoid + dwCurOffset, &dwStrSize, sizeof(ULONG));

			// Copy the actual Copyright String
			dwCurOffset += sizeof(ULONG);
			CopyMemory((BYTE*)pvoid + dwCurOffset,m_pCollection->m_Info.m_csCopyright.GetBuffer(0),dwStrSize);

			dwCurIndex++;
			m_pCollection->m_Info.m_csCopyright.ReleaseBuffer();
		}
		else
		{
			pDMWave->ulCopyrightIdx = 0;
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		hr = E_UNEXPECTED;
	}

	if(FAILED(hr))
	{
		ZeroMemory(pvoid, m_dwDLSize);
	}
	return hr;
}

/* initializes the download port, using a new download ID */
HRESULT CMonoWave::DM_Init(IDirectMusicPortDownload* pIDMPortDownLoad)
{
	// Do nothing if we don't have a valid port to download to...
	if(pIDMPortDownLoad == NULL)
	{
		return E_FAIL;
	}

	DWORD dwDMid;
	HRESULT hr = pIDMPortDownLoad->GetDLId(&dwDMid, 1);
	if (FAILED(hr))
		{
#ifdef _DEBUG
		AfxMessageBox(IDS_ERR_DM_ID, MB_OK | MB_ICONEXCLAMATION);
		return hr;
#endif
		}

	return DM_Init(pIDMPortDownLoad, dwDMid);
}

/* initializes the download port, using dwDMID for its id */
HRESULT CMonoWave::DM_Init(IDirectMusicPortDownload* pIDMPortDownLoad, DWORD dwDMID)
{
	HRESULT	hr = E_FAIL;
	ASSERT(m_pWaveNode);
	CDLSComponent* pComponent = m_pWaveNode->GetComponent();
	ASSERT(pComponent);

	// Do nothing if we don't have a valid port to download to...
	if(pIDMPortDownLoad == NULL)
	{
		return E_FAIL;
	}

	hr = Size(m_dwDLSize);
	if (m_dwDLSize)
	{
		// Add prepend and append amount to download buffer size
		DWORD dwAppendValue = 0;
		if(FAILED(pComponent->GetAppendValueForPort(pIDMPortDownLoad, &dwAppendValue)))
			return E_FAIL;
		
		m_dwDLSize += dwAppendValue;

		IDirectMusicDownload* pIDMDownload;
		hr = pIDMPortDownLoad->AllocateBuffer(m_dwDLSize, &pIDMDownload);
		if (!SUCCEEDED(hr))
		{
			AfxMessageBox(IDS_ERR_INSUFFICIENT_MEMORY, MB_OK | MB_ICONEXCLAMATION);
			return E_OUTOFMEMORY;
		}

		// Allocate a download buffer if we don't have one previously allocated
		AllocatedPortBufferPair* pAllocatedBuffer = GetAllocatedBufferForPort(pIDMPortDownLoad);
		if(pAllocatedBuffer == NULL)
			{
			// Add to the list of allocated buffers
			pAllocatedBuffer = new AllocatedPortBufferPair(pIDMPortDownLoad, pIDMDownload);
			if(pAllocatedBuffer == NULL)
			{
				return E_OUTOFMEMORY;
			}

			m_lstAllocatedBuffers.AddTail(pAllocatedBuffer);
			}

        pAllocatedBuffer->SetDMID(dwDMID);
	}
	else
	{
#ifdef _DEBUG
		AfxMessageBox(IDS_ERR_ZERO_SIZE_WAV, MB_OK | MB_ICONEXCLAMATION);
#endif
		hr = E_FAIL;
	}
	return hr;
}

HRESULT CMonoWave::Download(IDirectMusicPortDownload* pIDMPortDownLoad)
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);

	void*		pBuffer=NULL;
	DWORD		dwSize;
	HRESULT		hr = S_OK;
	
	// Do nothing if we don't have a valid port to download to...
	if(pIDMPortDownLoad == NULL)
	{
		return E_FAIL;
	}

	// Don't download anything if the wave is zero length
	if(m_dwWaveLength == 0)
	{
		return S_OK;
	}

	// Get the allocated buffer
	AllocatedPortBufferPair* pAllocatedBuffer = GetAllocatedBufferForPort(pIDMPortDownLoad);
	ASSERT(pAllocatedBuffer);
	
	// There isn't any buffer allocated 
	// most likely due to insufficient memory
	if(pAllocatedBuffer == NULL)
	{
		return E_FAIL;
	}

	IDirectMusicDownload* pIDMDownload = pAllocatedBuffer->m_pAllocatedBuffer;
	// Fail if we haven't allocated a buffer for download
	if(pIDMDownload == NULL)
	{
		return E_FAIL;
	}

	hr = pIDMDownload->GetBuffer(&pBuffer,&dwSize);
	if ( !SUCCEEDED(hr) || (pBuffer == NULL) || (dwSize < m_dwDLSize) )
	{
#ifdef _DEBUG
		AfxMessageBox(IDS_ERR_GET_DOWNLOAD_BUFFER, MB_OK | MB_ICONEXCLAMATION);
#endif
		return E_FAIL;
	}
	
	if(!SUCCEEDED(hr = Write((((BYTE*)pBuffer)), pIDMPortDownLoad)))
	{
#ifdef _DEBUG
		AfxMessageBox(IDS_ERR_DOWNLOAD_WRITE, MB_OK | MB_ICONEXCLAMATION);
#endif
		return hr;
	}
	if ( !SUCCEEDED( hr = pIDMPortDownLoad->Download(pIDMDownload)) )
	{
#ifdef _DEBUG
		UINT	iErrorStringResource;
		switch (hr)
		{
			case DMUS_E_BADWAVE:   // Bad wave chunk
				iErrorStringResource = IDS_ERR_BADWAVE;
				break;
			case DMUS_E_NOTPCM:    // Not PCM data in wave
				iErrorStringResource = IDS_ERR_NOTPCM;
				break;
			case DMUS_E_NOTMONO:	// Wave not MONO
				iErrorStringResource = IDS_ERR_NOTMONO;
				break;
			case DMUS_E_BADARTICULATION: // Bad articulation chunk
				iErrorStringResource = IDS_ERR_BADARTICULATION;
				break;
			case DMUS_E_BADWAVELINK:  // Bad link from region to wave
				iErrorStringResource = IDS_ERR_BADWAVELINK;
				break;
			case DMUS_E_BADINSTRUMENT:// Bad instrument chunk
				iErrorStringResource = IDS_ERR_BADINSTRUMENT;
				break;
			case DMUS_E_NOARTICULATION:// No articulation found in region
				iErrorStringResource = IDS_ERR_NOARTICULATION;
				break;
			case DMUS_E_BADOFFSETTABLE:
				iErrorStringResource = IDS_ERR_BADOFFSET;
				break;
			default:
				iErrorStringResource = IDS_ERR_DM_DOWNLOAD;
		}
		AfxMessageBox(iErrorStringResource, MB_OK | MB_ICONEXCLAMATION);
#endif
	}
	else if( m_pDummyInstrument )
	{
		ASSERT(m_pWaveNode);
		CDLSComponent* pComponent = m_pWaveNode->GetComponent();
		ASSERT(pComponent);

		POSITION position = pComponent->m_lstDLSPortDownload.GetHeadPosition();
		while(position)
		{
			IDirectMusicPortDownload* pIDMPortDownload = pComponent->m_lstDLSPortDownload.GetNext(position);
			ASSERT(pIDMPortDownload);
			m_pDummyInstrument->DM_Init(pIDMPortDownload);
			m_pDummyInstrument->Download(pIDMPortDownload);
		}
		
		m_pDummyInstrument->UpdatePatch();
		// HACK!! For some reason the performance requires that we send a PMSG for the patch to get applied...
		// HACK!! So we send the messages and hope when we hit play it's going to play the right wave and
		// HACK!! NOT that Skeletor laugh patch again!
		pComponent->PlayMIDIEvent(MIDI_NOTEON,
									  (BYTE)m_rWSMP.usUnityNote, //use wave's unity note
									  0,
									  0, // time
									  m_pDummyInstrument->IsDrum());   
		pComponent->PlayMIDIEvent(MIDI_NOTEOFF,
									  (BYTE)m_rWSMP.usUnityNote, //use wave's unity note
									  0,
									  0, // time
									  m_pDummyInstrument->IsDrum());   
	}
	return hr;
}


HRESULT CMonoWave::UpdateDecompressedWave(const WAVEFORMATEX* pwfxDst)
{
	ASSERT(pwfxDst);
	if(pwfxDst == NULL)
		return E_FAIL;

	if(m_bCompressed)
	{
		// We have to decompress the wave
		if(SUCCEEDED(ConvertWave(m_CompressedWavObj, pwfxDst, m_DecompressedWavObj, false))) 
		{
			// Clear the earlier wav
			m_pnWave = (short*)m_DecompressedWavObj.pbData;
			m_dwDataSize = m_DecompressedWavObj.dwDataSize;
			
			if(m_OriginalWavObj.WaveFormatEx.wBitsPerSample == 8)
				m_dwWaveLength = m_dwDataSize ;
			else
				m_dwWaveLength = m_dwDataSize >> 1;

			return S_OK;
		}
		else
		{
			SwitchCompression(false);
			return E_FAIL;
		}
	}

	return E_FAIL;
}


// bSwitch = false -> Compression OFF
// bSwitch = true -> Compression ON
HRESULT CMonoWave::SwitchCompression(bool bSwitch)
{
	HRESULT hr = S_OK;

	SetWaveBufferToCompressed(bSwitch);

    if(SUCCEEDED(hr))
    {
        // Update the header stream
	    // Stereo waves will do their own update
	    if(IsStereo() == FALSE)
	    {
		    if(SUCCEEDED(UpdateHeaderStream()))
		    {
			    m_pWaveNode->ReloadDirectSoundWave();
		    }
	    }
    }

    // Update the draw buffer
	RefreshDrawBuffer();

	CWaveCtrl* pWaveEditor = m_pWaveNode->GetWaveEditor();
    if(pWaveEditor)
    {
		pWaveEditor->InvalidateControl(); // Repaint the wave with new values
    }

	return hr;
}

void CMonoWave::SetWaveBufferToCompressed(bool bSwitch)
{
	m_bCompressed = bSwitch;
	
	if(!bSwitch)
	{
		m_dwDataSize = m_OriginalWavObj.dwDataSize;

		if(m_OriginalWavObj.WaveFormatEx.wBitsPerSample == 8)
		{
			m_dwWaveLength = m_dwDataSize ;
		}
		else
		{
			m_dwWaveLength = m_dwDataSize >> 1;
		}

		CopyMemory(&m_rWaveformat, &(m_OriginalWavObj.WaveFormatEx), sizeof(m_OriginalWavObj.WaveFormatEx));
	}
	else 
	{
		m_dwDataSize = m_DecompressedWavObj.dwDataSize;

		if(m_DecompressedWavObj.WaveFormatEx.wBitsPerSample == 8)
		{
			m_dwWaveLength = m_dwDataSize ;
		}
		else
		{
			m_dwWaveLength = m_dwDataSize >> 1;
		}

		CopyMemory(&m_rWaveformat, &(m_DecompressedWavObj.WaveFormatEx), sizeof(m_DecompressedWavObj.WaveFormatEx));
	}
}


// This is called if the m_pnWave member is modified directly...e.g. the clipboard 
// operations modify this pointer directly...
// So we need to update our wave objects accordingly
HRESULT CMonoWave::OnWaveBufferUpdated()
{
	if(m_DecompressedWavObj.pbData)
	{
		delete[] m_DecompressedWavObj.pbData;
		m_DecompressedWavObj.pbData = NULL;

		m_DecompressedWavObj.dwDataSize = 0;
	}

	if(FAILED(UpdateWave()))
	{
		return E_FAIL;
	}

	// Update the dummy instrument
	UpdateDummyInstrument();

	// clear the draw buffers
	RefreshDrawBuffer();
	if (m_pdbWaveTrack)
		m_pdbWaveTrack->Dirty();

	if(IsStereo() == false)
	{
		if(FAILED(CWave::OnWaveBufferUpdated()))
		{
			TRACE("DirectSoundWave header update failed!");
			return E_FAIL;
		}
	}

	// Update the data size
	if(m_bCompressed == false)
	{
		m_OriginalWavObj.dwDataSize = m_dwDataSize;
	}

	return S_OK;
}

HRESULT CMonoWave::Unload(IDirectMusicPortDownload* pIDMPortDownLoad)
{
	HRESULT hr = E_FAIL;
	
	ASSERT(pIDMPortDownLoad);
	if(pIDMPortDownLoad == NULL)
	{
		return E_POINTER;
	}

	// unload the dummy instrument first
	if(m_pDummyInstrument)
	{
		if(FAILED(m_pDummyInstrument->Unload(pIDMPortDownLoad)))
		{
			return E_FAIL;
		}
	}

	// now unload the wave
	AllocatedPortBufferPair* pAllocatedBuffer = GetAllocatedBufferForPort(pIDMPortDownLoad);
	if(pAllocatedBuffer == NULL)
	{
		return S_OK;
	}

	if(pAllocatedBuffer->m_pAllocatedBuffer)
	{
		hr = pAllocatedBuffer->m_pDownloadPort->Unload(pAllocatedBuffer->m_pAllocatedBuffer);
	}

	RemoveFromAllocatedBufferList(pAllocatedBuffer);

	return hr;
}

AllocatedPortBufferPair* CMonoWave::GetAllocatedBufferForPort(IDirectMusicPortDownload* pIDMPortDownLoad) const
{
	ASSERT(pIDMPortDownLoad);
	if(pIDMPortDownLoad == NULL)
		return NULL;

	POSITION position = m_lstAllocatedBuffers.GetHeadPosition();
	while(position)
	{
		AllocatedPortBufferPair* pAllocatedBuffer = (AllocatedPortBufferPair*)m_lstAllocatedBuffers.GetNext(position);
		
		ASSERT(pAllocatedBuffer);
		if(pAllocatedBuffer->m_pDownloadPort == pIDMPortDownLoad)
			return pAllocatedBuffer;
	}

	return NULL;
}

void CMonoWave::RemoveFromAllocatedBufferList(AllocatedPortBufferPair* pAllocatedBuffer)
{
	ASSERT(pAllocatedBuffer);
	if(pAllocatedBuffer == NULL)
		return;

	POSITION position = m_lstAllocatedBuffers.GetHeadPosition();

	while(position)
	{
		POSITION oldPosition = position;
		AllocatedPortBufferPair* pAllocatedBufferItem = (AllocatedPortBufferPair*)m_lstAllocatedBuffers.GetNext(position);
		if(pAllocatedBufferItem == pAllocatedBuffer)
		{
			m_lstAllocatedBuffers.RemoveAt(oldPosition);
			pAllocatedBufferItem->m_pAllocatedBuffer->Release();
			delete pAllocatedBufferItem;
			return;
		}
	}
}

void CMonoWave::SetPreviousRegionLinkID(DWORD dwID)
{
	m_dwPreviousRegionLinkID = dwID;
}

DWORD CMonoWave::GetDownloadID(IDirectMusicPortDownload* pIDMPortDownLoad) const
{
    DWORD dwDMID;
	GetDMIDForPort(pIDMPortDownLoad, &dwDMID);
    return dwDMID;
}


int CMonoWave::GetNumberOfLoops() const
{
	return m_rWSMP.cSampleLoops;
}

ULONG CMonoWave::GetWavePoolOffset()
{
	return m_dwHeaderOffset;
}


HRESULT CMonoWave::Compress(const WAVEFORMATEX* pwfxDest, bool bInUndoRedo)
{
	ASSERT(pwfxDest);
	if(pwfxDest == NULL)
	{
		return E_POINTER;
	}

	ASSERT(m_pDataManager);
	if(m_pDataManager == NULL)
	{
		return E_FAIL;
	}

    // Update the uncompressed deltas if we're not already compressed
    if(!m_bCompressed && !bInUndoRedo)
    {
        if(FAILED(m_pDataManager->UpdateUncompressedDeltas()))
        {
            return E_FAIL;
        }
    }

	// Ask the data manager to recreate the decomp file
	if(FAILED(m_pDataManager->CreateDecompFile()))
	{
		return E_FAIL;
	}

	int nBytesPerSample = m_OriginalWavObj.WaveFormatEx.wBitsPerSample / 8;
    DWORD dwDataSize = m_OriginalWavObj.dwDataSize;
    DWORD dwWaveLength = (nBytesPerSample == 1) ? dwDataSize  : (dwDataSize >> 1);
    
	// Remember the old wavelength
	DWORD dwOldLength = dwWaveLength;

	// We have to compress the wave in chunks
	DWORD dwSamplesCompressed = 0;
	DWORD dwSamplesToCompress = dwWaveLength;
	
	DWORD dwCompressedSize = 0;
	DWORD dwDecompressedSize = 0;

	// Get data in chunks of WAVE_DATA_BUFFER_SIZE 
	while(dwSamplesCompressed < dwWaveLength)
	{
		BYTE* pbData = NULL;
		//DWORD dwLength = WAVE_DATA_BUFFER_SIZE;
		DWORD dwLength = dwWaveLength;
		dwSamplesToCompress = dwWaveLength - dwSamplesCompressed;
		
		// Take the big slice for the last chunk....
		// The conversion might fail if we take very small amount of data
		if(dwSamplesToCompress <= WAVE_DATA_BUFFER_SIZE * 2)
		{
			dwLength = dwSamplesToCompress;
		}

		if(FAILED(GetChannelData(dwSamplesCompressed, dwLength, &pbData, true)))
		{
			return E_FAIL;
		}

		// Now compress the data
		DWORD dwSourceSize = dwLength * nBytesPerSample;
		BYTE* pbCompressed = NULL;
		DWORD dwCompressed = 0;
		if(FAILED(ConvertWave(&m_rWaveformat, pwfxDest, pbData, dwSourceSize, &pbCompressed, dwCompressed)))
		{
			delete[] pbData;
			return E_FAIL;
		}

		dwCompressedSize += dwCompressed;
		
		// Update the number of samples successfully compressed
		dwSamplesCompressed += dwLength;

		// Now decompress the compressed data and give it to data manager
		BYTE* pbDecompressed = NULL;
		DWORD dwDecompressed = 0;
		if(FAILED(ConvertWave(pwfxDest, &m_rWaveformat, pbCompressed, dwCompressed, &pbDecompressed, dwDecompressed)))
		{
			delete[] pbData;
			delete[] pbCompressed;
			return E_FAIL;
		}

		dwDecompressedSize += dwDecompressed;
		
		delete[] pbData;
		delete[] pbCompressed;

		// Ask the data manager to write the decompressed buffer to the decomp file
		if(FAILED(m_pDataManager->WriteDecompressedData(pbDecompressed, dwDecompressed)))
		{
			delete[] pbDecompressed;
			return E_FAIL;
		}

		delete[] pbDecompressed;
	}

	// Reset the compression info
	CleanWaveObject(&m_CompressedWavObj);
	m_CompressedWavObj.dwDataSize = dwCompressedSize;
	CopyMemory(&(m_CompressedWavObj.WaveFormatEx), pwfxDest, sizeof(WAVEFORMATEX));
	if(pwfxDest->cbSize)
	{
		if(FAILED(SafeAllocBuffer(&(m_CompressedWavObj.pbExtractWaveFormatData), pwfxDest->cbSize)))
		{
			return E_OUTOFMEMORY;
		}
		CopyMemory(m_CompressedWavObj.pbExtractWaveFormatData, ((BYTE*)pwfxDest) + sizeof(WAVEFORMATEX), pwfxDest->cbSize);
	}

	// Reset the decompression info	
	CleanWaveObject(&m_DecompressedWavObj);
	m_DecompressedWavObj.dwDataSize = dwDecompressedSize;
	CopyMemory(&(m_DecompressedWavObj.WaveFormatEx), &m_rWaveformat, sizeof(WAVEFORMATEX));


	// Our wavelength has possibly changed 
	m_dwDataSize = dwDecompressedSize;
	m_dwWaveLength = dwDecompressedSize / nBytesPerSample;
	
	// Mark the wave as compressed...
	m_bCompressed = true;

	// Update the loop points if it's looped...
	if(m_bPlayLooped)
	{
		LONG lDifference = m_dwWaveLength - dwOldLength;
		m_rWLOOP.ulStart += lDifference;
		m_rWLOOP.ulStart = m_rWLOOP.ulStart;
		m_rWLOOP.ulStart = min(m_rWLOOP.ulStart, m_dwWaveLength);
		m_rWLOOP.ulLength = min(m_rWLOOP.ulLength, m_dwWaveLength - m_rWLOOP.ulStart);
		
		m_rRLOOP.dwStart = m_rWLOOP.ulStart;
		m_rRLOOP.dwEnd = m_rWLOOP.ulStart + m_rWLOOP.ulLength - 1;
	}

	return S_OK;
}


HRESULT CMonoWave::WriteCompressedData(IStream* pIStream, WAVEFORMATEX* pwfxDest, DWORD& cbWritten)
{
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_POINTER;
	}

	ASSERT(pwfxDest);
	if(pwfxDest == NULL)
	{
		return E_POINTER;
	}

	ASSERT(m_pDataManager);
	if(m_pDataManager == NULL)
	{
		return E_FAIL;
	}

	int nBytesPerSample = m_rWaveformat.wBitsPerSample / 8;

	// We have to compress the wave in chunks
	DWORD dwSamplesCompressed = 0;
	DWORD dwSamplesToCompress = m_OriginalWavObj.dwDataSize / nBytesPerSample;
	DWORD dwUncompressedLength = dwSamplesToCompress;
	
	DWORD dwCompressedSize = 0;

	// Get data in chunks of WAVE_DATA_BUFFER_SIZE 
	while(dwSamplesCompressed < dwUncompressedLength)
	{
		BYTE* pbData = NULL;
		DWORD dwLength = dwUncompressedLength;
		dwSamplesToCompress = dwUncompressedLength - dwSamplesCompressed;
		
		// Take the big slice for the last chunk....
		// The conversion might fail if we take very small amount of data
		if(dwSamplesToCompress <= WAVE_DATA_BUFFER_SIZE * 2)
		{
			dwLength = dwSamplesToCompress;
		}

		if(FAILED(GetChannelData(dwSamplesCompressed, dwLength, &pbData, true)))
		{
			return E_FAIL;
		}

		// Now compress the data
		DWORD dwSourceSize = dwLength * nBytesPerSample;
		BYTE* pbCompressed = NULL;
		DWORD dwCompressed = 0;
		if(FAILED(ConvertWave(&m_rWaveformat, pwfxDest, pbData, dwSourceSize, &pbCompressed, dwCompressed)))
		{
			delete[] pbData;
			return E_FAIL;
		}

		dwCompressedSize += dwCompressed;
		
		// Update the number of samples successfully compressed
		dwSamplesCompressed += dwLength;

		// Write the data to the stream
		DWORD cb = 0;
		if(FAILED(pIStream->Write(pbCompressed, dwCompressed, &cb)))
		{
			delete[] pbData;
			return E_FAIL;
		}

		delete[] pbData;
		delete[] pbCompressed;

		cbWritten += cb;	
	}

	return S_OK;
}


void CMonoWave::OnDraw(CDC* pDC, const CRect& rcClient, const long lOffset, DWORD& dwStartSample, DWORD dwLength, float fSamplesPerPixel, const WaveDrawParams* pDrawParams, const WaveTimelineInfoParams* pInfoParams)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Nothing to draw?
	if(dwLength == 0 )
	{
		return;
	}


	ASSERT(pDC);
	if(pDC == NULL)
	{
		return;
	}

	ASSERT(pDrawParams);
	if(pDrawParams == NULL)
	{
		return;
	}

	ASSERT(pInfoParams);
	if(pInfoParams == NULL)
	{
		return;
	}

	if(dwStartSample >= m_dwWaveLength)
	{
		return;
	}

	// Check the rect for 0 width
	if(rcClient.Width() <= 0)
	{
		return;
	}

	BOOL bLoopedOnTrack = pInfoParams->dwFlags & WAVE_LOOPED;
	DWORD dwLoopStart = (pInfoParams->dwLoopStart) / DRAW_UNIT_SIZE;
	DWORD dwLoopEnd = pInfoParams->dwLoopEnd / DRAW_UNIT_SIZE;

	int nMidY = (rcClient.bottom - rcClient.top) / 2; 

	dwStartSample = dwStartSample;
	if(m_dwWaveLength < dwStartSample + dwLength && !bLoopedOnTrack)
	{
		dwLength = m_dwWaveLength - dwStartSample;
	}

	DWORD dwStart = dwStartSample / DRAW_UNIT_SIZE;
	DWORD dwDrawLength = (dwLength < DRAW_UNIT_SIZE) ? 1 : dwLength / DRAW_UNIT_SIZE;

	// What's our sample size?
	int nBytesPerSample = m_rWaveformat.wBitsPerSample / 8;

	// What's our zero value? (We assume only 8 and 16 bit waves)
	int nZeroValue = nBytesPerSample == 1 ? 128 : 0;

	// What's our max Y value?
	float fYScale = nBytesPerSample == 1 ? (float) nMidY / 128 : (float) nMidY / 32768;
	
	// What's our screen width?
	int nRectWidth = rcClient.Width();
	
	// How many samples to step in each iteration?
	float nSampleStep = (fSamplesPerPixel > 1) ? fSamplesPerPixel : 1;

	// How many pixels do we have to skip per sample?
	float nXStep = (fSamplesPerPixel >= 1) ? 1 : ((float)nRectWidth / dwDrawLength); 

	float fAccumulatedXStepErr = 0;
	float fXStepErr = nXStep - (int)nXStep;
	int nSampleSkip = (int)(1 / fXStepErr);
	if(nXStep < 1 && lOffset == 0)
	{
		fXStepErr = 1 - nXStep;
	}
	
	// Need to take into account the error while drawing...
	float fAccumulatedError = 0;
	float fErrorFactor = fSamplesPerPixel - (int) fSamplesPerPixel;
	if(fSamplesPerPixel < 1)
	{
		fErrorFactor = 0;
	}

	// create the draw buffer on the fly
	if (m_pdbWaveTrack == NULL)
		{
		m_pdbWaveTrack = new CMonoWaveDrawBuffer(mwdbTypeWaveTrack);
		if (m_pdbWaveTrack == NULL)
			return;
		m_pdbWaveTrack->Init(DRAW_UNIT_SIZE, this);
		}

	// fill visible area of the wave track draw buffer
	DWORD dwUnitStart = dwStart;
	DWORD dwUnitLength = dwDrawLength;
	if ((bLoopedOnTrack) && (dwLoopStart < dwStart))
		{
		// we may have to go further back because of a loop
		// assume worst case: start at loop, and end at end of current visible screen
		dwUnitLength += dwStart - dwLoopStart;
		dwUnitStart = dwLoopStart;
		}
    BYTE* pbData;
	DWORD dwDataLength;
	if (FAILED(m_pdbWaveTrack->Get(dwUnitStart*DRAW_UNIT_SIZE, dwUnitLength*DRAW_UNIT_SIZE, &pbData, &dwDataLength)))
		return;

	COLORREF clrWave = pDrawParams->clrForeground;
	COLORREF clrBgnd = pDrawParams->clrBackground;
	COLORREF clrClip = pDrawParams->clrClipMarker;

	// Fill the rect with the background color
	if(pDrawParams->bErase)
		pDC->FillSolidRect(rcClient, clrBgnd);
		
	// Draw the LIGHT-BLUE zero-line.
	pDC->FillSolidRect(rcClient.left, rcClient.top+nMidY, rcClient.Width(), 1, RGB(192, 176, 244));

	// Temp fix for DX8 Bug 44791. The value 255 is defined in WaveString.cpp as
	// COLOR_SELECTED_WAVE. Because it is defined in the cpp file, we will have
	// to hard code the value here.
	CPen wavePen(PS_SOLID, 0, clrWave);
	CPen bluePen(PS_SOLID, 0, RGB(0, 0, 164));
	CPen* pOldPen = pDC->SelectObject((m_bCompressed && (255 != clrWave)) ? &bluePen : &wavePen);

	if(nBytesPerSample == 1)
	{
		// We're reading bytes here.
        
		// Move to the first visible sample.
        int nY = (int)(fYScale * (abs(pbData[0] - nZeroValue)));
        pDC->MoveTo(rcClient.left, (pbData[0] > nZeroValue) ? (rcClient.top + (nMidY - nY)) : (rcClient.top + (nMidY + nY)));

		DWORD dwSampleIndex = 0;
		for(int nXPos = rcClient.left + (int)(nXStep); nXPos < rcClient.right; nXPos += (int)floor(nXStep))
		{
			dwSampleIndex += (DWORD) nSampleStep;
			if(bLoopedOnTrack && dwSampleIndex >= dwLoopEnd)
			{
				dwSampleIndex = dwLoopStart;
			}

			if(fAccumulatedError > 1)
			{
				dwSampleIndex++;
				fAccumulatedError -= 1;
			}

			if(fAccumulatedXStepErr > 1)
			{
				nXPos++;
				fAccumulatedXStepErr -= 1;
			}

			if(dwSampleIndex >= dwDataLength)
			{
				if(bLoopedOnTrack)
				{
					dwSampleIndex = dwLoopStart;
				}
				else
				{
					break;
				}
			}
			
			int nSampleValue = pbData[dwSampleIndex];
			int nNextSampleValue = nSampleValue > 128 ? 128 - (nSampleValue - 128) : 128 + (128 - nSampleValue);

			if (nSampleValue < 128)
			{                    
				pDC->LineTo(nXPos, rcClient.top + nMidY + (int)(float)(fYScale * (128 - abs(nSampleValue))));
				pDC->LineTo(nXPos, rcClient.top + nMidY + (int)(float)(fYScale * (128 - abs(nNextSampleValue))));
			}
			else
			{
				pDC->LineTo(nXPos, rcClient.top + nMidY - (int)((float)(fYScale * (nSampleValue - 128))));                       
				pDC->LineTo(nXPos, rcClient.top + nMidY - (int)((float)(fYScale * (nNextSampleValue - 128))));                       
			}

			fAccumulatedError += fErrorFactor;
			fAccumulatedXStepErr += fXStepErr;
		}
	}
	else
	{
		short* pnDrawBuffer = (short*)pbData;

		// Move to the first visible sample.
        int nY = (int)(fYScale * (abs(pnDrawBuffer[0] - nZeroValue)));
        pDC->MoveTo( rcClient.left, (pnDrawBuffer[0] > nZeroValue) ? (rcClient.top + (nMidY - nY)) : (rcClient.top + (nMidY + nY)));

		DWORD dwSampleIndex = 0;
		for(int nXPos = rcClient.left + (int)nXStep; nXPos < rcClient.right; nXPos += (int)floor(nXStep))
		{
			dwSampleIndex += (DWORD) nSampleStep;
			if(bLoopedOnTrack && dwSampleIndex >= dwLoopEnd)
			{
				dwSampleIndex = dwLoopStart;
			}

			if(fAccumulatedError > 1)
			{
				dwSampleIndex++;
				fAccumulatedError -= 1;
			}

			if(fAccumulatedXStepErr > 1)
			{
				nXPos++;
				fAccumulatedXStepErr -= 1;
			}

			if(dwSampleIndex >= dwDataLength)
			{
				if(bLoopedOnTrack)
				{
					dwSampleIndex = dwLoopStart;
				}
				else
				{
					break;
				}
			}
			
			int nSampleValue = pnDrawBuffer[dwSampleIndex];
			int nNextSampleValue = -nSampleValue;

			if (nSampleValue < 0)
			{                    
				pDC->LineTo(nXPos, rcClient.top + nMidY + (int)(float)(fYScale * abs(nSampleValue)));
				pDC->LineTo(nXPos, rcClient.top + nMidY + (int)(float)(fYScale * abs(nNextSampleValue)));
			}
			else
			{
				pDC->LineTo(nXPos, rcClient.top + nMidY - (int)((float)(fYScale * nSampleValue)));                       
				pDC->LineTo(nXPos, rcClient.top + nMidY - (int)((float)(fYScale * nNextSampleValue)));                       
			}

			fAccumulatedError += fErrorFactor;
			fAccumulatedXStepErr += fXStepErr;
		}
	}
	
	// Select the old GDIObjects back
	if(pOldPen)
	{
		pDC->SelectObject(pOldPen);
	}
}


void CMonoWave::OnDraw(CDC* pDC, const CRect& rcClient)
{
	// Must have a DC!
	ASSERT(pDC);
	if(!pDC)
	{
		return;
	}

	// Must have an editor to draw on!
	CWaveCtrl* pWaveEditor = GetWaveEditor();
	ASSERT(pWaveEditor);
	if(pWaveEditor == NULL)
	{
		return;
	}

	int nMidY = (rcClient.bottom - rcClient.top) / 2; 

	// How many samples per pixel do we draw?
	int nSamplesPerPixel = pWaveEditor->GetSamplesPerPixel();

	// How much are we scrolled?
	int nSamplesScrolled = pWaveEditor->GetScrolledSamples();
	
	// How much are we zoomed in?
	int nZoomFactor = pWaveEditor->GetZoomFactor();

	// What's our maximum height
	int nyMax = ((rcClient.bottom - rcClient.top) - 2) / 2; 

	// How many samples do we have to draw?
    int nSamplesToDraw = min((int)m_dwWaveLength - nSamplesScrolled, (nZoomFactor >= 0) ?
		rcClient.right * nSamplesPerPixel :
		(rcClient.right / nSamplesPerPixel) + 2); // +1 because first sample is used at start, +1 to draw line past end

	// What's our maximum width?
    int nxMax = (nZoomFactor >= 0) ? nSamplesToDraw / nSamplesPerPixel : nSamplesToDraw * nSamplesPerPixel;

	// switch zoom buffers if necessary
	int nSamplesPerPixelZoom = (nZoomFactor > 0) ? nSamplesPerPixel : 1;
	if ((m_pdb == NULL) || (m_pdb->GetSamplesPerPixel() != nSamplesPerPixelZoom))
		{
		// compute optimum sample rate on the fly
		if (m_nSamplesPerPixelOptimum == 0)
			{
			// compute maximum samples per pixel (whole wave fits in screen, estimated at 1K pixels)
			double dMaxSampsPerPixel = ((double)m_dwWaveLength / 1000);
			short nZoomFactorMin = (short)ceil(log(dMaxSampsPerPixel) / log(2));

			// the "optimum" is four zooms above the minimum zoom, and between 16 and 512 samples per pixel
			// (16 to not be ridiculous and remember data that can be quickly read from disk)
			// (512 to speed up browsing of large waves)
			if (nZoomFactorMin >= 4)
				{
				m_nSamplesPerPixelOptimum = 1 << (nZoomFactorMin - 4); // same as dividing max sample/pixel rate by 16
				if (m_nSamplesPerPixelOptimum > 512)
					m_nSamplesPerPixelOptimum = 512;
				else if (m_nSamplesPerPixelOptimum < 16)
					m_nSamplesPerPixelOptimum = 16;
				}
			else
				m_nSamplesPerPixelOptimum = 16; // no buffer needed: 16 will never be reached, max is 1 << 3 = 8 for this wave
			}

		// destroy "latest" buffer if it exists
		if (m_pdbLatest)
			{
			delete m_pdbLatest;
			m_pdbLatest = NULL;
			}

		// build the optimum zoom buffer if needed now
		if ((nSamplesPerPixelZoom >= (int)m_nSamplesPerPixelOptimum) && (m_pdbOptimum == NULL))
			{
			m_pdbOptimum = new CMonoWaveDrawBuffer(mwdbTypeWaveEditor);
			if (m_pdbOptimum == NULL)
				return;
			m_pdbOptimum->Init(m_nSamplesPerPixelOptimum, this);
			}

		// create draw buffer on the fly
		if (nSamplesPerPixelZoom == (int)m_nSamplesPerPixelOptimum)
			{
			ASSERT(m_pdbOptimum->GetSamplesPerPixel() == (int)m_nSamplesPerPixelOptimum);
			m_pdb = m_pdbOptimum;
			}
		else
			{
			m_pdbLatest = new CMonoWaveDrawBuffer(mwdbTypeWaveEditor);
			if (m_pdbLatest == NULL)
				return;
			if (nSamplesPerPixelZoom > (int)m_nSamplesPerPixelOptimum)
				m_pdbLatest->Init(nSamplesPerPixelZoom, m_pdbOptimum);
			else
				m_pdbLatest->Init(nSamplesPerPixelZoom, this);
			m_pdb = m_pdbLatest;
			}
		}

	// fill draw buffer
	short* pnData;
	DWORD dwUnitLength;
	ASSERT(m_pdb);
	ASSERT(m_pdb->GetSamplesPerPixel() == nSamplesPerPixelZoom);
	if (FAILED(m_pdb->Get(nSamplesScrolled, nSamplesToDraw, (BYTE **)&pnData, &dwUnitLength)))
		return;
	
	// This might be a new wave with no data in it yet....
	if(pnData == NULL)
		return;

	// Draw the LIGHT-BLUE zero-line.
	pDC->FillSolidRect(rcClient.left, rcClient.top+nyMax, rcClient.Width(), 1, RGB(192, 176, 244));

	CPen blackPen(PS_SOLID, 0, RGB(0, 0, 0));
	CPen bluePen(PS_SOLID, 0, RGB(0, 0, 164));
	CPen *pOldPen = pDC->SelectObject(m_bCompressed ? &bluePen : &blackPen);

	int nXStep = (nZoomFactor > 0) ? 1 : nSamplesPerPixel;
	DWORD dwSample = 0;
	if(m_rWaveformat.wBitsPerSample == 8)
    {
		// We're reading bytes here.
        BYTE* pByte = (BYTE*) pnData;
        
		// 128 is the zero value for 8-bit data
        float nyScale = (float) nyMax / 128;

        // Move to the first visible sample.
        int ny = (int)(nyScale * (abs(pByte[0] - 128)));
        pDC->MoveTo( rcClient.left, (pByte[0] > 128) ? (rcClient.top + (nyMax - ny)) : (rcClient.top + (nyMax + ny)));

		for(int nXPos = 0; nXPos <= nxMax; nXPos += nXStep)
        {
			if (dwSample >= dwUnitLength)
				break;

            if (pByte[dwSample] > 128)
                pDC->LineTo(nXPos, rcClient.top + nyMax - (int)(nyScale * (pByte[dwSample] - 128)));
            else    
                pDC->LineTo(nXPos, rcClient.top + nyMax + (int)(nyScale * (128 - pByte[dwSample])));

			dwSample++;
        }

	}
	else if(m_rWaveformat.wBitsPerSample == 16)
    {
		short* pData = pnData;
        
		float nyScale = (float) nyMax / 32768;
		
		// Move to the first visible sample.
		int ny = (int)((float)(nyScale * (abs(pData[0]))));
        pDC->MoveTo(rcClient.left, (pData[0] > 0) ? (rcClient.top + (nyMax - ny)) : (rcClient.top + (nyMax + ny)));

		for(int nXPos = 0; nXPos <= nxMax; nXPos += nXStep)
        {
			if (dwSample >= dwUnitLength)
				break;

            if (pData[dwSample] < 0)
                pDC->LineTo(nXPos, rcClient.top + nyMax + (int)(float)(nyScale * abs(pData[dwSample])));
            else
                pDC->LineTo(nXPos, rcClient.top + nyMax - (int)((float)(nyScale * pData[dwSample])));                       

			dwSample++;
        }
	}

	// Select the old GDIObjects back
	if(pOldPen)
	{
		pDC->SelectObject(pOldPen);
	}
}

void CMonoWave::RefreshDrawBuffer()
{
	if (m_pdbOptimum)
		m_pdbOptimum->Dirty();
	if (m_pdbLatest)
		m_pdbLatest->Dirty();
}

HRESULT CMonoWave::GetChannelData(DWORD dwStartSample, DWORD dwLength, BYTE** ppbData, bool bGetUncompressedOnly)
{
	if(m_dwWaveLength == 0 || dwLength == 0)
	{
		return S_OK;
	}

    HRESULT hr = E_FAIL;

	if(IsStereo() && m_pCollection && m_bCompressed)
	{
		DWORD cb = 0;

        EnterCriticalSection(&m_pWaveNode->m_CriticalSection);
		hr = GetData(dwStartSample, dwLength, ppbData, cb, bGetUncompressedOnly);
        LeaveCriticalSection(&m_pWaveNode->m_CriticalSection);
        return hr;
	}
	else if(IsStereo())
	{
		CStereoWave* pStereoWave = (CStereoWave*)(m_pWaveNode->GetWave());
		ASSERT(pStereoWave);
		if(pStereoWave == NULL)
		{
			return E_FAIL;
		}

		if(pStereoWave->IsTrueStereo())
		{
			DWORD cb = 0;
            EnterCriticalSection(&m_pWaveNode->m_CriticalSection);
            hr = pStereoWave->GetData(dwStartSample, dwLength, ppbData, cb, bGetUncompressedOnly);
            LeaveCriticalSection(&m_pWaveNode->m_CriticalSection);
			if(FAILED(hr))
			{
				return E_FAIL;
			}

			// Strip the channels data
			BYTE* pbChannelData = NULL;
			DWORD dwDataSize = cb / 2;
			if(FAILED(SafeAllocBuffer(&pbChannelData, dwDataSize)))
			{
				delete[] *ppbData;
				return E_OUTOFMEMORY;
			}

			if(FAILED(StripChannelData(*ppbData, cb, pbChannelData)))
			{
				delete[] *ppbData;
				return E_FAIL;
			}

			delete[] *ppbData;
			*ppbData = pbChannelData;
			return hr;
		}
	}

	DWORD cb = 0;
    EnterCriticalSection(&m_pWaveNode->m_CriticalSection);
    hr = GetData(dwStartSample, dwLength, ppbData, cb, bGetUncompressedOnly);
    LeaveCriticalSection(&m_pWaveNode->m_CriticalSection);
	return hr;
}


HRESULT CMonoWave::GetAllUncompressedChannelData(BYTE** ppbData, DWORD* pdwSize)
{
    ASSERT(pdwSize);
    if(pdwSize == NULL)
    {
        return E_POINTER;
    }
    
    if(m_dwWaveLength == 0)
	{
		return S_OK;
	}
    
    bool bUncompressedInTrueStereo = false;
    if(IsStereo())
    {
        CStereoWave* pStereoWave = (CStereoWave*)(m_pWaveNode->GetWave());
		ASSERT(pStereoWave);
		if(pStereoWave == NULL)
		{
			return E_FAIL;
		}

		bUncompressedInTrueStereo = (pStereoWave->IsTrueStereo() != 0);
    }

    if(bUncompressedInTrueStereo == false)
	{
		return GetAllUncompressedData(ppbData, pdwSize);
	}
    else
	{
		CStereoWave* pStereoWave = (CStereoWave*)(m_pWaveNode->GetWave());
		ASSERT(pStereoWave);
		if(pStereoWave == NULL)
		{
			return E_FAIL;
		}

		if(pStereoWave->IsTrueStereo())
		{
			DWORD cb = 0;
			if(FAILED(pStereoWave->GetAllUncompressedData(ppbData, pdwSize)))
			{
				return E_FAIL;
			}

			// Strip the channels data
			BYTE* pbChannelData = NULL;
			DWORD dwDataSize = *pdwSize / 2;
			if(FAILED(SafeAllocBuffer(&pbChannelData, dwDataSize)))
			{
				delete[] *ppbData;
				return E_OUTOFMEMORY;
			}

			if(FAILED(StripChannelData(*ppbData, *pdwSize, pbChannelData)))
			{
				delete[] *ppbData;
				return E_FAIL;
			}

			delete[] *ppbData;
			*ppbData = pbChannelData;
            *pdwSize = dwDataSize;
			return S_OK;
		}
	}

	return E_FAIL;
}


HRESULT CMonoWave::StripChannelData(const BYTE* pbSrcData, DWORD dwChunkSize, BYTE* pbDestBuffer)
{
	// All pointers must be valid
	ASSERT(pbSrcData);
	if(pbSrcData == NULL)
		return E_POINTER;

	ASSERT(pbDestBuffer);
	if(pbDestBuffer == NULL)
		return E_POINTER;

	int nBytesPerSample = m_rWaveformat.wBitsPerSample / 8;
	int nInitialPos = 0;
	if(m_bRightChannel)
	{
		nInitialPos = nBytesPerSample;
	}

	for(DWORD dwByteCount = nInitialPos; dwByteCount < dwChunkSize; dwByteCount += nBytesPerSample * 2)
	{
		CopyMemory(pbDestBuffer, pbSrcData + dwByteCount, nBytesPerSample);
		pbDestBuffer += nBytesPerSample;
	}

	/* TODO: Enable this optimization
	const BYTE *pbEndSrc = pbSrcData + dwChunkSize;

	if( 16 == m_rWaveformat.wBitsPerSample )
	{
		if(m_bRightChannel)
		{
			pbSrcData += 2;
		}

		while( pbSrcData < pbEndSrc )
		{
			CopyMemory(pbDestBuffer, pbSrcData, 2);
			pbSrcData += 4;
			pbDestBuffer += 2;
		}
	}
	else if( 8 == m_rWaveformat.wBitsPerSample )
	{
		if(m_bRightChannel)
		{
			pbSrcData += 1;
		}

		while( pbSrcData < pbEndSrc )
		{
			CopyMemory(pbDestBuffer, pbSrcData, 1);
			pbSrcData += 2;
			pbDestBuffer += 1;
		}
	}
	else
	{
		const int nBytesPerSample = m_rWaveformat.wBitsPerSample / 8;

		if(m_bRightChannel)
		{
			pbSrcData += nBytesPerSample;
		}

		while( pbSrcData < pbEndSrc )
		{
			CopyMemory(pbDestBuffer, pbSrcData, nBytesPerSample);
			pbSrcData += nBytesPerSample * 2;
			pbDestBuffer += nBytesPerSample;
		}
	}
	*/

	return S_OK;
}

DWORD CMonoWave::GetDataSize()
{
	return m_dwDataSize;
}

DWORD CMonoWave::GetUncompressedDataSize()
{
	return m_OriginalWavObj.dwDataSize;
}

DWORD CMonoWave::GetCompressedDataSize()
{
	return m_CompressedWavObj.dwDataSize;
}


DWORD CMonoWave::GetCompressionFormatTag()
{
	ASSERT(m_bCompressed);
	if(m_bCompressed)
	{
		return m_CompressedWavObj.WaveFormatEx.wFormatTag;
	}
	
	return 0;
}

HRESULT CMonoWave::GetCompressionFormat(WAVEFORMATEX* pwfxCompression)
{
	ASSERT(pwfxCompression);
	if(pwfxCompression == NULL)
		return E_POINTER;

	CopyMemory(pwfxCompression, &m_CompressedWavObj.WaveFormatEx, sizeof(WAVEFORMATEX));
	return S_OK;
}


BYTE* CMonoWave::GetCompressionFormatExtraInfo()
{
	return m_CompressedWavObj.pbExtractWaveFormatData;
}


GUID CMonoWave::GetGUID()
{
	return m_guidStereoWave;
}

void CMonoWave::SetGUID(const GUID& guidStereoWave)
{
	CWaveNode::CopyGUID(guidStereoWave, m_guidStereoWave);
}

// Caller will have to deal with updating the wave
// The method just does what it says...removes a bunch of bytes
HRESULT CMonoWave::RemoveSelection(DWORD dwStart, DWORD dwEnd)
{
	// Swap the points if necessary
	DWORD dwTemp = dwStart;
	if(dwStart > dwEnd)
	{
		DWORD dwTemp = dwStart;
		dwStart = dwEnd;
		dwEnd = dwTemp;
	}

	DWORD dwBytesRemoved;
	DWORD dwSamplesRemoved = abs(dwEnd - dwStart) + 1;

	DWORD dwNewBufferSize = 0;
	if (m_rWaveformat.wBitsPerSample == 8)
	{
		dwBytesRemoved = dwEnd - dwStart + 1;
	}
	else
	{
		dwBytesRemoved = (dwEnd << 1) - (dwStart << 1) + 2;
	}

    dwNewBufferSize = m_OriginalWavObj.dwDataSize - dwBytesRemoved;

	if(FAILED(m_pDataManager->RemoveData(dwStart, dwSamplesRemoved)))
	{
		return E_FAIL;
	}

    m_OriginalWavObj.dwDataSize -= dwBytesRemoved;
	m_dwDataSize = m_OriginalWavObj.dwDataSize;
	m_dwWaveLength -= dwSamplesRemoved;

	return S_OK;
}

HRESULT CMonoWave::CopySelectionToClipboard(IStream* pIStream, DWORD dwStart, DWORD dwEnd)
{
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_POINTER;
	}

	// Swap the points if necessary
	if(dwStart > dwEnd)
	{
		DWORD dwTemp = dwStart;
		dwStart = dwEnd;
		dwEnd = dwTemp;
	}

	DWORD dwClipLength = dwEnd - dwStart + 1;

	if(FAILED(CWave::WriteDataToClipboard(pIStream, dwStart, dwClipLength)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonoWave::PasteSelectionFromClipboard(IStream* pIStream, DWORD dwStart)
{
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_POINTER;
	}

	BYTE* pbData = NULL;
	WAVEFORMATEX wfxClip;
	DWORD dwDataSize = 0;
	if(FAILED(CWave::ReadDataFromClipbaord(pIStream, wfxClip, &pbData, dwDataSize)) || pbData == NULL || dwDataSize == 0)
	{
		return E_FAIL;
	}

	UINT nClipSampleSize = wfxClip.wBitsPerSample / 8;
	UINT nClipChannels = wfxClip.nChannels;

	DWORD dwClipLength = (dwDataSize / nClipSampleSize) / nClipChannels;
	
	HRESULT hr = InsertWaveData(pbData, wfxClip, dwClipLength, dwStart);
	if(FAILED(hr))
	{
		delete[] pbData;
	}

	return hr;
}


HRESULT CMonoWave::InsertWaveData(BYTE* pbData, WAVEFORMATEX wfFormat, DWORD  dwSamples, DWORD dwStart)
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	DWORD dwNewBufferSize = 0;
	DWORD dwNumNewBytes = 0;

	UINT nClipChannels = wfFormat.nChannels;

	if(nClipChannels == 2)
	{
		BYTE* pbMonoData = NULL;
		int nSampleSize = wfFormat.wBitsPerSample / 8;

		DWORD dwMonoSize = dwSamples * nSampleSize;
		if(FAILED(SafeAllocBuffer(&pbMonoData, dwMonoSize)))
		{
			return E_OUTOFMEMORY;
		}

		if(nSampleSize == 2)
		{
			if(FAILED(MixStereoToMono((short*)pbData, (short*)pbMonoData, dwSamples)))
			{
				delete[] pbMonoData;
				return E_FAIL;
			}
		}
		else if(nSampleSize == 1)
		{
			if(FAILED(MixStereoToMono(pbData, pbMonoData, dwSamples)))
			{
				delete[] pbMonoData;
				return E_FAIL;
			}
		}
		else
		{
			// We can't handle any other sample sizes
			delete[] pbMonoData;
			return E_FAIL;
		}
		
		delete[] pbData;
		pbData = pbMonoData;
		nClipChannels = 1;
	}
	else if(nClipChannels != 1)
	{
		return E_FAIL;
	}

	if(m_rWaveformat.wBitsPerSample == 8)
	{
		dwNumNewBytes = dwSamples * nClipChannels;
		//dwNewBufferSize = m_dwWaveLength + dwNumNewBytes;
	}
	else
	{
		dwNumNewBytes = (dwSamples * nClipChannels) << 1;
		//dwNewBufferSize = (m_dwWaveLength << 1) + dwNumNewBytes;
	}

    dwNewBufferSize = m_OriginalWavObj.dwDataSize + dwNumNewBytes;

	if(m_rWaveformat.wBitsPerSample != wfFormat.wBitsPerSample)
	{
		DWORD dwIndex = 0;
		
		if(m_rWaveformat.wBitsPerSample == 8) 
		{
			BYTE* pbNewBuffer = NULL;
			if(FAILED(SafeAllocBuffer(&pbNewBuffer, dwNumNewBytes)))
			{
				return E_OUTOFMEMORY;
			}

			for(dwIndex = 0; dwIndex < dwNumNewBytes; dwIndex++)
			{
				pbNewBuffer[dwIndex] = *((short*)pbData + dwIndex) < 0 ? 
					128 - MulDiv(abs(*((short*)pbData + dwIndex)), 128, 32768) :
					128 + MulDiv(abs(*((short*)pbData + dwIndex)), 127, 32767);
			}

			delete[] pbData;
			pbData = pbNewBuffer;
		}
		else
		{
			short* pnNewBuffer = NULL;
			if(FAILED(SafeAllocBuffer((BYTE**)&pnNewBuffer, dwNumNewBytes)))
			{
				return E_OUTOFMEMORY;
			}

			for(dwIndex = 0; dwIndex < dwSamples; dwIndex++)
			{
				pnNewBuffer[dwIndex] = *(pbData + dwIndex) < 128 ? 
					- MulDiv(abs(*(pbData + dwIndex) - 128), 32768, 128) :
					+ MulDiv(*(pbData + dwIndex) - 128, 32767, 127);
			}

			delete[] pbData;
			pbData = (BYTE*)pnNewBuffer;
		}
	}

	if(FAILED(m_pDataManager->InsertData(dwStart, (BYTE*)pbData, dwSamples)))
	{
		return E_FAIL;
	}

	delete[] pbData;
	
    m_OriginalWavObj.dwDataSize = dwNewBufferSize;
    m_dwDataSize = m_OriginalWavObj.dwDataSize; 
    m_dwWaveLength += dwSamples;

	return S_OK;
}


HRESULT CMonoWave::MergeStereoDataBuffers(short* pbLeftChannelData, short* pbRightChannelData, WAVEFORMATEX wfxClipFormat, short* pbMergedData, DWORD dwDataSize)
{
	if(pbLeftChannelData == NULL)
		return E_POINTER;
	if(pbRightChannelData == NULL)
		return E_POINTER;
	if(pbMergedData == NULL)
		return E_POINTER;

	int nBytesPerSample = 1;
	if(wfxClipFormat.wBitsPerSample == 16)
		nBytesPerSample = 2;
	
	for(int nCount = 0; nCount < (int)(dwDataSize / 2); nCount += nBytesPerSample)
	{
		short dwMergedSampleValue = 0;
		if(wfxClipFormat.wBitsPerSample == 8)
		{
			dwMergedSampleValue = (pbLeftChannelData[nCount] + pbRightChannelData[nCount]) / 2;
			pbMergedData[nCount] = (BYTE)dwMergedSampleValue;
		}
		else
		{
			dwMergedSampleValue = (pbLeftChannelData[nCount] + pbRightChannelData[nCount]) / 2;
			pbMergedData[nCount] = dwMergedSampleValue;
		}
	}

	return S_OK;
}

HRESULT CMonoWave::InsertSilence(DWORD dwStart, DWORD dwLength)
{
	if( dwLength == 0 )
	{
		return S_OK;
	}

	DWORD dwBytesPerSample = 1;
	if(m_rWaveformat.wBitsPerSample > 8)
	{
		dwBytesPerSample = m_rWaveformat.wBitsPerSample / 8;
	}

	BYTE* pbBuffer = new BYTE[dwLength * dwBytesPerSample];
	if( !pbBuffer )
	{
		return E_FAIL;
	}

	ZeroMemory( pbBuffer, dwLength * dwBytesPerSample );

	HRESULT hr = InsertWaveData(pbBuffer, m_rWaveformat, dwLength, dwStart);
	if(FAILED(hr))
	{
		delete[] pbBuffer;
		return E_FAIL;
	}

	// If inserted before loop start point
	if( dwStart < m_rWLOOP.ulStart )
	{
		// Update the start and end values
		SetLoop( m_rRLOOP.dwStart + dwLength, m_rRLOOP.dwEnd + dwLength );
	}
	// If inserted before loop end point
	else if( dwStart < m_rWLOOP.ulStart + m_rWLOOP.ulLength )
	{
		// Update end value (start remains the same)
		SetLoop( m_rRLOOP.dwStart, m_rRLOOP.dwEnd + dwLength );
	}

	return S_OK;
}

HRESULT CMonoWave::CopyLoop()
{
	// Wave's not looped?
	if(m_bPlayLooped == false)
	{
		return E_FAIL;
	}

	DWORD dwLoopLength = m_rWLOOP.ulLength;
	DWORD dwStartSample = m_rWLOOP.ulStart;
	DWORD dwEndSample = dwStartSample + dwLoopLength;

	BYTE* pbBuffer = NULL;
	if(FAILED(GetChannelData(dwStartSample, dwLoopLength, &pbBuffer)))
	{
		return E_FAIL;
	}

	DWORD dwStart = dwEndSample;
	DWORD dwEnd = dwStart;
	HRESULT hr = InsertWaveData(pbBuffer, m_rWaveformat, dwLoopLength, dwStart);
	if(FAILED(hr))
	{
		delete[] pbBuffer;
		return E_FAIL;
	}

	// Move the loop values to the copied position now...
	m_rWLOOP.ulStart = dwEndSample;
	m_rRLOOP.dwStart = dwEndSample;
	m_rRLOOP.dwEnd = dwEndSample + dwLoopLength;

	UpdateLoopInRegions();

	return S_OK;
}

short* CMonoWave::GetWaveData()
{
	BYTE* pbData = NULL;
	if(FAILED(GetChannelData(0, m_dwWaveLength, &pbData)))
	{
		return NULL;
	}

	return (short*)pbData;
}

HRESULT CMonoWave::UpdateDataForStereoCompression(const DMUSP_WAVEOBJECT& stereoWaveObject)
{
	CleanWaveObject(&m_DecompressedWavObj);
	
	DWORD dwDataSize = stereoWaveObject.dwDataSize;

	m_dwDataSize = dwDataSize / 2;
	m_bCompressed = true;	
	CopyMemory(&m_DecompressedWavObj.WaveFormatEx, &(stereoWaveObject.WaveFormatEx), sizeof(WAVEFORMATEX));
	m_DecompressedWavObj.dwDataSize = m_dwDataSize;

	CopyMemory(&m_rWaveformat, &(stereoWaveObject.WaveFormatEx), sizeof(WAVEFORMATEX));
	m_rWaveformat.nChannels = 1;
	if(((m_rWaveformat.wBitsPerSample == 8) && (m_rWaveformat.nBlockAlign == 2)) ||
	  ((m_rWaveformat.wBitsPerSample == 16) && (m_rWaveformat.nBlockAlign == 4)))
	{
		m_rWaveformat.nBlockAlign = m_rWaveformat.nBlockAlign / 2;
	}
	if (m_rWaveformat.wBitsPerSample == 8)
	{
		m_dwWaveLength = m_dwDataSize;
	}
	else
	{
		m_dwWaveLength = m_dwDataSize >> 1;
	}

	return S_OK;
}

HRESULT CMonoWave::SetUncompressedData(const DMUSP_WAVEOBJECT& stereoWaveObject)
{
	CleanWaveObject(&m_OriginalWavObj);
	
	DWORD dwDataSize = stereoWaveObject.dwDataSize;

	m_OriginalWavObj.dwDataSize = dwDataSize / 2;
	CopyMemory(&m_OriginalWavObj.WaveFormatEx, &(stereoWaveObject.WaveFormatEx), sizeof(WAVEFORMATEX));

	m_OriginalWavObj.WaveFormatEx.nChannels = 1;
	if(((m_OriginalWavObj.WaveFormatEx.wBitsPerSample == 8) && (m_OriginalWavObj.WaveFormatEx.nBlockAlign == 2)) ||
	  ((m_OriginalWavObj.WaveFormatEx.wBitsPerSample == 16) && (m_OriginalWavObj.WaveFormatEx.nBlockAlign == 4)))
	{
		m_OriginalWavObj.WaveFormatEx.nBlockAlign = m_OriginalWavObj.WaveFormatEx.nBlockAlign / 2;
	}

	if(m_bCompressed == false)
	{
		m_dwDataSize = m_OriginalWavObj.dwDataSize;
		if (m_OriginalWavObj.WaveFormatEx.wBitsPerSample == 8)
		{
			m_dwWaveLength = m_dwDataSize;
		}
		else
		{
			m_dwWaveLength = m_dwDataSize >> 1;
		}
	}

	return S_OK;

}

HRESULT	CMonoWave::GetPlayTimeForTempo(int nTempo, DWORD& dwPlayTime)
{
	// Normal tempo is 120 bpm
	float fNormalPlayTime = (float) m_dwWaveLength / m_rWaveformat.nSamplesPerSec;
	dwPlayTime = (DWORD)(nTempo * (fNormalPlayTime / 120));

	return S_OK;
}

void CMonoWave::SnapToZero(DWORD& dwSample, UINT nDirection)
{
	// We don't care about start and end samples
	if(dwSample == 0 || dwSample == m_dwWaveLength - 1)
		return;

	bool fForward = (nDirection & SNAP_FORWARD) != 0;
	bool fBackward = (nDirection & SNAP_BACKWARD) != 0;
	ASSERT(fForward || fBackward); // at least one direction must be specified
	BYTE* pbData;

	// start with typical case: zero crossing is within 512 samples around start position

	// find boundaries before and after the start sample
	const int dwTypicalLength = 512;
	DWORD dwLengthBefore = fBackward ?
		(dwSample > dwTypicalLength) ?
			dwTypicalLength :				// room to grow on the left
			dwSample :						// clip to start of wave
		0;									// no need to go left
	DWORD dwLengthAfter = fForward ?
		(dwSample+dwTypicalLength <= m_dwWaveLength) ?
			dwTypicalLength :				// room to grow on the right
			m_dwWaveLength-dwSample :		// clip to end of wave
		0;									// no need to go right

	// get data
	ASSERT(dwSample >= dwLengthBefore);
	ASSERT(dwLengthBefore+dwLengthAfter > 0);
	if (FAILED(GetChannelData(dwSample-dwLengthBefore, dwLengthBefore+dwLengthAfter, &pbData)))
		return;

	// look for zero crossing
	int iSnap = FindSnapToZero(pbData, dwLengthBefore, 0, dwLengthBefore+dwLengthAfter, nDirection);
	delete[] pbData;
	if (iSnap != 0)
		{
		dwSample += iSnap;
		return;
		}

	// not found, expand search in each direction
	// note overlap of 2 on each buffer:
	// - we need to compare the last of the previous buffer, to the first of the current one -> overlap 1
	// - FindSnapToZero always skips the first sample -> overlap 1 more
	DWORD dwOffset = dwTypicalLength-2;
	int iSnapBefore = 0;
	int iSnapAfter = 0;
	while (fBackward || fForward)
		{
		ASSERT((iSnapBefore == 0) && (iSnapAfter == 0)); // we should have exited otherwise
		
		// look forward
		if (fForward)
			{
			DWORD dwStart = dwSample+dwOffset;
			if (dwStart > m_dwWaveLength)
				fForward = false; // reached end, stop looking
			else
				{
				DWORD dwLength = dwTypicalLength;
				if (dwStart+dwLength > m_dwWaveLength)
					dwLength = m_dwWaveLength-dwStart;
					
				if (FAILED(GetChannelData(dwStart, dwLength, &pbData)))
					return;
				iSnapAfter = FindSnapToZero(pbData, 0, 0, dwLength, SNAP_FORWARD);
				delete[] pbData;
				}
			}

		// look backward
		if (fBackward)
			{
			if (dwSample < dwOffset)
				fBackward = false; // reached start, stop looking
			else
				{
				DWORD dwStart = dwSample-dwOffset;
				DWORD dwLength = dwTypicalLength;
				if (dwStart < dwLength)
					dwLength = dwStart;
					
				if (FAILED(GetChannelData(dwStart, dwLength, &pbData)))
					return;
				iSnapBefore = FindSnapToZero(pbData, dwLength, 0, dwLength, SNAP_BACKWARD);
				delete[] pbData;
				}
			}

		// return if we found snap values, pick the smallest in absolute value (closest to start sample)
		if ((iSnapBefore != 0) || (iSnapAfter != 0))
			{
			ASSERT(iSnapBefore <= 0);
			ASSERT(iSnapAfter >= 0);
			if ((iSnapBefore < 0) && (iSnapAfter > 0))
				if (-iSnapBefore < iSnapAfter)
					iSnapAfter = 0;
				else
					iSnapBefore = 0;
			if (iSnapBefore < 0)
				dwSample -= dwOffset - iSnapBefore;
			else
				dwSample += dwOffset + iSnapAfter;
			return;
			}

		// prepare for next loop
		dwOffset += dwTypicalLength-2;
		}

	// no zero crossing found across entire wave in the given direction(s)
}

/* looks for the zero crossing start at dwSample, in the given nDirection, within the limits of dwLeft and dwRight.
	If found, returns the offset from dwSample in iOffset. Returns 0 if not found. */
int CMonoWave::FindSnapToZero(BYTE *pbData, DWORD dwSample, DWORD dwLeft, DWORD dwRight, UINT nDirection)
{
	int nCount = 1;
	if(m_rWaveformat.wBitsPerSample == 8)
    {
        BYTE* pByte = (BYTE*) pbData;
		while ((dwSample >= dwLeft+nCount) || (dwSample+nCount < dwRight))
        {
            if((nDirection & SNAP_FORWARD) && (dwSample + nCount + 1) < (int)dwRight)
            {
                if(pByte[dwSample + nCount] >= 128 && pByte[dwSample + nCount + 1] < 128)
                	return nCount + 1;
            }

            if((nDirection & SNAP_BACKWARD) && dwSample >= (dwLeft+nCount+1))
            {  
				// check in the other direction.
                if(pByte[dwSample - nCount] < 128 && pByte[dwSample - nCount - 1] >= 128)
                	return -nCount;
            }

			nCount++;
        } 
	}
	else
    {
		short* pnData = (short*) pbData;
		while ((dwSample >= dwLeft+nCount) || (dwSample+nCount < dwRight))
		{
			if((nDirection & SNAP_FORWARD) && (dwSample + nCount + 1) < (int)dwRight)
			{
				if( pnData[dwSample + nCount] >= 0 && pnData[dwSample + nCount + 1] < 0 )
					return nCount+1;
			}

			if((nDirection & SNAP_BACKWARD) && dwSample >= (dwLeft+nCount+1))
			{
				// Check in the other direction.
				if(pnData[dwSample - nCount] < 0 && pnData[dwSample - nCount - 1] >= 0)
					return -nCount;
			}

			nCount++;
		} 
	}

	return false; // not found
}

void CMonoWave::UpdateOnUndo(HEADER_INFO headerInfo)
{
	CWave::UpdateOnUndo(headerInfo);
	if(IsStereo() == FALSE)
	{
		if(SUCCEEDED(UpdateHeaderStream()))
		{
			m_pWaveNode->ReloadDirectSoundWave();
		}
	}
}


HRESULT	CMonoWave::WriteRIFFHeaderToStream(IStream* pIStream, DWORD* pdwBytesWritten)
{
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_POINTER;
	}

	ASSERT(pdwBytesWritten);
	if(pdwBytesWritten == NULL)
	{
		return E_POINTER;
	}

	// Seek to the begiinning of this stream...
	if(FAILED(StreamSeek(pIStream, 0, STREAM_SEEK_SET)))
	{
		return E_FAIL;
	}

	IDMUSProdRIFFStream* pIRiffStream = NULL;
	if(FAILED(AllocRIFFStream(pIStream, &pIRiffStream)))
	{
		return E_OUTOFMEMORY;
	}

	MMCKINFO ckMain;
	ckMain.fccType = mmioFOURCC('W','A','V','E');
	
	// RIFF chunk size = 'WAVE' + 'wavhXXXX' + sizeof(DMUS_IO_WAVE_HEADER) + 'fmt XXXX' + PCM-WAVEFORMATEX + 'dataXXXX' + actual data size
	ckMain.cksize = 4 + 8 + sizeof(DMUS_IO_WAVE_HEADER) + 8 + sizeof(WAVEFORMATEX) + 8 + m_dwDataSize;

	HRESULT hr = pIRiffStream->CreateChunk(&ckMain, MMIO_CREATERIFF);
    if(FAILED(hr))
	{
		pIRiffStream->Release();
        return (hr);
    }


	// Save the 'wavh' chunk that keeps the streaming info
	MMCKINFO ck;
	ck.ckid = DMUS_FOURCC_WAVEHEADER_CHUNK;
	hr = pIRiffStream->CreateChunk(&ck, 0);
    if (FAILED(hr))
    {
		pIStream->Release();
        return hr;
    }

	_DMUS_IO_WAVE_HEADER waveHeader;
	if(FAILED(CWave::GetStreamingWaveHeader(&waveHeader)))
	{
		pIStream->Release();
		return E_FAIL;
	}

	DWORD cb = 0;
	hr = pIStream->Write((LPSTR)&waveHeader, sizeof(_DMUS_IO_WAVE_HEADER), &cb);
	if(FAILED(hr) || cb != sizeof(_DMUS_IO_WAVE_HEADER))
	{
		pIStream->Release();
		return E_FAIL;
	}
	
	if(FAILED(pIRiffStream->Ascend(&ck, 0)))
	{
		pIStream->Release();
		return E_FAIL;
	}


	if(FAILED(WriteFormat(pIRiffStream, false)))
	{
		pIRiffStream->Release();
		return E_FAIL;
	}

	// Keep the offset before writing the data-ck
	DWORD dwOffset = StreamTell(pIStream);

	// Write only upto <data-ck>[size]
	ck.ckid = mmioFOURCC('d','a','t','a');
	ck.cksize = m_dwDataSize;
	if(FAILED(pIRiffStream->CreateChunk(&ck,0)))
	{
		pIRiffStream->Release();
		return E_FAIL;
	}

	// Don't ascend the data chunk as we're not writing any wave data
	// We just need to have the correct data size set in the chunk 
	DWORD dwBytesFromStart = StreamTell(pIStream);
	StreamSeek(pIStream, dwOffset, STREAM_SEEK_SET);

	if(FAILED(StreamSeek(pIStream, 0, STREAM_SEEK_SET)))
	{
		return E_FAIL;
	}

	*pdwBytesWritten = dwBytesFromStart;
	pIRiffStream->Release();

	return S_OK;
}


HRESULT CMonoWave::WriteFormat(IDMUSProdRIFFStream* pIRiffStream, bool bRuntime)
{
	ASSERT(pIRiffStream);
	if(pIRiffStream == NULL)
	{
		return E_POINTER;
	}

	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_FAIL;
	}

	MMCKINFO ck;
	ck.ckid = mmioFOURCC('f','m','t',' ');
	HRESULT hr = pIRiffStream->CreateChunk(&ck, 0);
	if(FAILED(hr))
	{
		pIStream->Release();
		return hr;
	}

	// Run Time -> We need to save the compressed wav format
	if(bRuntime && m_bCompressed)  
	{
		DWORD cb = 0;
		hr = pIStream->Write((LPSTR) &(m_CompressedWavObj.WaveFormatEx), sizeof(WAVEFORMATEX), &cb);
		if (FAILED(hr) || cb != sizeof(WAVEFORMATEX))
		{
			pIStream->Release();
			return E_FAIL;
		}

		// Save the extra information with format info
		hr = pIStream->Write((LPSTR) m_CompressedWavObj.pbExtractWaveFormatData, m_CompressedWavObj.WaveFormatEx.cbSize, &cb); 
		if (FAILED(hr) || cb != m_CompressedWavObj.WaveFormatEx.cbSize)
		{
			pIStream->Release();
			return E_FAIL;
		}
	}
	else // Design Time -> Save the original uncompressed wav format
	{
		DWORD cb = 0;
		hr = pIStream->Write((LPSTR) &(m_OriginalWavObj.WaveFormatEx),sizeof(WAVEFORMATEX), &cb);	
		if (FAILED(hr) || cb != sizeof(WAVEFORMATEX))
		{
			pIStream->Release();
			return E_FAIL;
		}
	}

	if (FAILED(pIRiffStream->Ascend(&ck, 0)))
	{
		pIStream->Release();
		return E_FAIL;
	}

	pIStream->Release();

	return S_OK;
}


HRESULT	CMonoWave::WriteCompressionChunk(IDMUSProdRIFFStream* pIRiffStream, bool bRuntime)
{
	ASSERT(pIRiffStream);
	if(pIRiffStream == NULL)
	{
		return E_POINTER;
	}

	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_FAIL;
	}

	MMCKINFO ck;
	ck.ckid = mmioFOURCC('w','a','v','u'); 
	HRESULT hr = pIRiffStream->CreateChunk(&ck, 0);
	if(FAILED(hr))
	{
		pIStream->Release();
		return (hr);
	}

	// Save the flag that marks whether this file is Runtime or Designtime
	DWORD cb = 0;
	hr = pIStream->Write((LPSTR) &(bRuntime), sizeof(bool), &cb);	
	if(FAILED(hr) || cb != sizeof(bool))
	{
		pIStream->Release();
		return E_FAIL;
	}

	// Save the flag that marks compression
	hr = pIStream->Write((LPSTR) &(m_bCompressed), sizeof(bool), &cb);	
	if(FAILED(hr) || cb != sizeof(bool))
	{
		pIStream->Release();
		return E_FAIL;
	}

	// Save the compression format in the wavu chunk if this is not a runtime save
	if(m_bCompressed && bRuntime == false)
	{
		// Save the format header
		hr = pIStream->Write((LPSTR) &(m_CompressedWavObj.WaveFormatEx), sizeof(WAVEFORMATEX), &cb);	
		if(FAILED(hr) || cb != sizeof(WAVEFORMATEX))
		{
			pIStream->Release();
			return E_FAIL;
		}
		
		// Compressed waves might have extra information in the header
		if(m_CompressedWavObj.WaveFormatEx.cbSize)
		{
			hr = pIStream->Write(m_CompressedWavObj.pbExtractWaveFormatData, m_CompressedWavObj.WaveFormatEx.cbSize, &cb);
			if(FAILED(hr) || cb != m_CompressedWavObj.WaveFormatEx.cbSize)
			{
				pIStream->Release();
				return E_FAIL;
			}
		}
	}

	// But if it is the runtime save then we'd rather preserve the original wave format to uncompress it properly
	if(m_bCompressed && bRuntime)
	{
		// Save the format header
		hr = pIStream->Write((LPSTR) &(m_OriginalWavObj.WaveFormatEx), sizeof(WAVEFORMATEX), &cb);	
		if (FAILED(hr) || cb != sizeof(WAVEFORMATEX))
		{
			pIStream->Release();
			return E_FAIL;
		}
		
		// Compressed waves might have extra information in the header
		if(m_OriginalWavObj.WaveFormatEx.cbSize)
		{
			hr = pIStream->Write(m_OriginalWavObj.pbExtractWaveFormatData, m_OriginalWavObj.WaveFormatEx.cbSize, &cb);
			if(FAILED(hr) || cb != m_OriginalWavObj.WaveFormatEx.cbSize)
			{
				pIStream->Release();
				return E_FAIL;
			}
		}
	}

    // Write the actual start of the decompressed data
    cb = 0;
    hr = pIStream->Write(&m_dwDecompressedStart, sizeof(DWORD), &cb);
    if(FAILED(hr) || cb != sizeof(DWORD))
    {
        pIStream->Release();
        return E_FAIL;
    }

	if (FAILED(pIRiffStream->Ascend(&ck, 0)))
	{
		pIStream->Release();
		return E_FAIL;
	}

	pIStream->Release();

	return S_OK;
}


HRESULT CMonoWave::WriteFACTChunk(IDMUSProdRIFFStream* pIRiffStream)
{
	ASSERT(pIRiffStream);
	if(pIRiffStream == NULL)
	{
		return E_POINTER;
	}

	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_FAIL;
	}

	MMCKINFO ck;
	ck.ckid = mmioFOURCC('f','a','c','t');
	HRESULT hr = pIRiffStream->CreateChunk(&ck, 0);
	if(FAILED(hr))
	{
		pIStream->Release();
		return (hr);
	}

	// Write the number of original samples (before compression)
	int nSampleSize = m_OriginalWavObj.WaveFormatEx.wBitsPerSample / 8;
	DWORD dwSamples = m_OriginalWavObj.dwDataSize / nSampleSize;
	
    DWORD cb = 0;
	hr = pIStream->Write((LPSTR) &(dwSamples), sizeof(DWORD), &cb);
	if(FAILED(hr) || cb != sizeof(DWORD))
	{
		pIStream->Release();
		return E_FAIL;
	}

	if (FAILED(pIRiffStream->Ascend(&ck, 0)))
	{
		pIStream->Release();
		return E_FAIL;
	}

	pIStream->Release();

	return S_OK;
}


HRESULT CMonoWave::WriteRSMPLChunk(IDMUSProdRIFFStream* pIRiffStream)
{
	ASSERT(pIRiffStream);
	if(pIRiffStream == NULL)
	{
		return E_POINTER;
	}

	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_FAIL;
	}

	MMCKINFO ck;
	ck.ckid = mmioFOURCC('s','m','p','l');
    HRESULT hr = pIRiffStream->CreateChunk(&ck, 0);
    if (FAILED(hr))
    {
		pIStream->Release();
        return (hr);
    }

    DWORD dwSaveSize = sizeof(RSMPL);
    if (m_rRSMPL.cSampleLoops > 0)
    {
        dwSaveSize += sizeof(RLOOP);
        m_rRSMPL.cSampleLoops = 1;
    }

	DWORD cb = 0;
    hr = pIStream->Write((LPSTR) &m_rRSMPL,dwSaveSize, &cb);
    if (FAILED(pIRiffStream->Ascend(&ck, 0)) || cb != dwSaveSize)
    {
		pIStream->Release();
        return E_FAIL;
    }

	pIStream->Release();

	return S_OK;
}

BOOL CMonoWave::IsTrueStereo()
{
	return FALSE;
}

HRESULT CMonoWave::CloseSourceHandler()
{
	ASSERT(m_pDataManager);
	if(m_pDataManager == NULL)
	{
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;
	EnterCriticalSection(&m_pWaveNode->m_CriticalSection);
	hr = m_pDataManager->CloseSourceHandler();
	LeaveCriticalSection(&m_pWaveNode->m_CriticalSection);

	return hr;
}


HRESULT CMonoWave::OnSourceRenamed(CString sNewName)
{
	ASSERT(m_pDataManager);
	if(m_pDataManager == NULL)
	{
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;
	EnterCriticalSection(&m_pWaveNode->m_CriticalSection);
	hr = m_pDataManager->OnSourceRenamed(sNewName);
	LeaveCriticalSection(&m_pWaveNode->m_CriticalSection);
	
	return hr;
}


HRESULT CMonoWave::FileNameChanged(CString sNewFileName)
{
	ASSERT(m_pDataManager);
	if(m_pDataManager == NULL)
	{
		return E_FAIL;
	}
	
	if(FAILED(m_pDataManager->Initialize(sNewFileName, m_dwHeaderOffset, m_dwDataOffset, false)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT	CMonoWave::UpdateHeaderStream()
{
	if(IsStereo() == false)
	{
		return CWave::UpdateHeaderStream();
	}

	return E_FAIL;
}

HRESULT CMonoWave::FindBestLoop(DWORD dwLoopStart, DWORD dwFindStart, DWORD dwFindEnd, int nFindWindow, DWORD* pdwBestLoopEnd)
{
	ASSERT(nFindWindow > 0);
	if(nFindWindow <= 0)
	{
		return E_FAIL;
	}

	dwFindEnd = dwFindEnd >= (m_dwWaveLength - (nFindWindow / 2)) ? ((m_dwWaveLength - (nFindWindow / 2)) - 1) : dwFindEnd;

	DWORD dwSourceStart = dwLoopStart - (nFindWindow / 2);
	if((int)dwSourceStart < 0)
	{
		dwSourceStart = 0;
	}

	BYTE* pbSourcePattern = NULL;
	if(FAILED(GetChannelData(dwSourceStart, nFindWindow, &pbSourcePattern)))
	{
		return E_FAIL;
	}

	if(dwFindStart <= dwLoopStart + (nFindWindow / 2))
	{
		dwFindStart = dwLoopStart + (nFindWindow / 2) + 1;
	}

	DWORD dwMinDiff = 0xFFFFFFFF;
	DWORD dwBestSample = dwFindStart;

	DWORD dwMatchStart = (dwFindStart - (nFindWindow / 2));
	while((dwMatchStart + (nFindWindow / 2) <= dwFindEnd) && (dwMatchStart + (nFindWindow / 2) < m_dwWaveLength))
	{
		BYTE* pbMatchPattern = NULL;
		if(FAILED(GetChannelData(dwMatchStart, nFindWindow, &pbMatchPattern)))
		{
			delete[] pbSourcePattern;
			pbSourcePattern = NULL;
			return E_FAIL;
		}

		int nSampleSize = m_rWaveformat.wBitsPerSample;
		
		DWORD dwDiff = 0;
		if(nSampleSize == 16)
		{
			dwDiff = DiffPatterns((short*)pbSourcePattern, (short*)pbMatchPattern, nFindWindow);
		}
		else
		{
			dwDiff = DiffPatterns(pbSourcePattern, pbMatchPattern, nFindWindow);
		}

		if(dwDiff <= dwMinDiff)
		{
			dwMinDiff = dwDiff;
			dwBestSample = dwMatchStart;
		}

		dwMatchStart++;
		
		delete[] pbMatchPattern;
		pbMatchPattern = NULL;
	}

	delete[] pbSourcePattern;
	*pdwBestLoopEnd = dwBestSample + ((nFindWindow / 2) - 1);
	return S_OK;
}

template <class T>
DWORD CMonoWave::DiffPatterns(T* pSourcePattern, T* pMatchPattern, DWORD dwPatternLength)
{
	DWORD dwDiff = 0;
	DWORD dwStart = 0;
	while(dwStart < dwPatternLength)
	{
		 dwDiff += abs(pSourcePattern[dwStart] - pMatchPattern[dwStart]);
		 dwStart++;
	}

	return dwDiff;
}

template <class T> 
HRESULT CMonoWave::MixStereoToMono(T* pStereoData, T* pMonoData, DWORD dwStereoLength)
{
	ASSERT(pStereoData);
	if(pStereoData == NULL)
	{
		return E_POINTER;
	}

	ASSERT(pMonoData);
	if(pMonoData == NULL)
	{
		return E_POINTER;
	}

	if(dwStereoLength == 0)
	{
		return S_OK;
	}

	DWORD dwStereoSample = 0;
	for(DWORD dwMonoSample = 0; dwMonoSample < dwStereoLength; dwMonoSample++)
	{
		pMonoData[dwMonoSample] = ((pStereoData[dwStereoSample] + pStereoData[dwStereoSample++]) / 2);
		dwStereoSample++;
	}

	return S_OK;
}

HRESULT CMonoWave::GetDMIDForPort(IDirectMusicPortDownload* pIDMDownloadPort, DWORD* pdwDMID) const
{
    ASSERT(pdwDMID);
    if(pdwDMID == NULL)
        return E_POINTER;

	AllocatedPortBufferPair* pBuffer = GetAllocatedBufferForPort(pIDMDownloadPort);
	if (!pBuffer)
		return E_FAIL;

	*pdwDMID = pBuffer->GetDMID();
	return S_OK;
}

BOOL CMonoWave::GetWaveForDownloadID(DWORD dwID)
{
    POSITION position = m_lstAllocatedBuffers.GetHeadPosition();
    while(position)
    {
        AllocatedPortBufferPair* pBuffer = m_lstAllocatedBuffers.GetNext(position);
        ASSERT(pBuffer);
        if(pBuffer->GetDMID() == dwID)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/* returns given channel */
CMonoWave *CMonoWave::GetChannel(LONG iChannel)
{
	if (iChannel == 0)
		return this;

	ASSERT(FALSE); // only one channel in a mono wave
	return NULL;
}

/* sets wavelink parameters according to channel number */
void CMonoWave::SetWaveLinkPerChannel(LONG iChannel, WAVELINK *pwl)
{
	ASSERT(pwl);
	ASSERT(iChannel == 0);
	pwl->ulTableIndex = GetWaveID();
}

