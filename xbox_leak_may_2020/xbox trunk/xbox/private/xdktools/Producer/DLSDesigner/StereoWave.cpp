//////////////////////////////////////////////////////////////////////
//
// StereoWave.cpp : implementation file
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <mmsystem.h>
#include <mmreg.h>
#include <msacm.h>
#include "DLSDesignerDLL.h"
#include "WaveNode.h"
#include "WaveDataManager.h"
#include "Wave.h"
#include "MonoWave.h"
#include "StereoWave.h"
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

 
CStereoWave::CStereoWave(CWaveNode* pWaveNode, BOOL bTrueStereo, DWORD dwSampleRate, UINT nSampleSize) : CWave(pWaveNode, dwSampleRate, nSampleSize, 2), 
m_bIsPlaying(FALSE), 
m_bTrueStereo(FALSE),
m_bLoadingRuntime(false)
{
	m_bTrueStereo = bTrueStereo;

	::CoCreateGuid(&m_guidStereoWave);
	m_pLeftChannel = new CMonoWave(pWaveNode, m_guidStereoWave, false);
	m_pRightChannel = new CMonoWave(pWaveNode, m_guidStereoWave, true);

	if(m_pDataManager == NULL)
	{
		CString sSourceFileName = m_pWaveNode->GetFileName();
		char szTempPath[MAX_PATH];
		DWORD dwSuccess = GetTempPath(MAX_PATH, szTempPath);
		m_pDataManager = new CWaveDataManager(this, sSourceFileName, szTempPath);
	}
}

CStereoWave::CStereoWave(CWaveNode* pWaveNode, GUID guidStereoWave, DWORD dwSampleRate, UINT nSampleSize) : CWave(pWaveNode, dwSampleRate, nSampleSize, 2), 
m_guidStereoWave(guidStereoWave),
m_bIsPlaying(FALSE),
m_bTrueStereo(FALSE),
m_bLoadingRuntime(false)
{
	m_pLeftChannel = new CMonoWave(pWaveNode, m_guidStereoWave, false);
	m_pRightChannel = new CMonoWave(pWaveNode, m_guidStereoWave, true);

	if(m_pDataManager == NULL)
	{
		CString sSourceFileName = m_pWaveNode->GetFileName();
		char szTempPath[MAX_PATH];
		DWORD dwSuccess = GetTempPath(MAX_PATH, szTempPath);
		m_pDataManager = new CWaveDataManager(this, sSourceFileName, szTempPath);
	}
}


CStereoWave::~CStereoWave()
{
	if(m_pLeftChannel)
	{
		delete m_pLeftChannel;
		m_pLeftChannel = NULL;
	}

	if(m_pRightChannel)
	{
		delete m_pRightChannel;
		m_pRightChannel = NULL;
	}
}
 

HRESULT CStereoWave::LoadHeader(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain)
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

	ASSERT(m_pLeftChannel);
	if(m_pLeftChannel == NULL)
	{
		return E_FAIL;
	}

	ASSERT(m_pRightChannel);
	if(m_pRightChannel == NULL)
	{
		return E_FAIL;
	}

	// This is a wave ina DLS collection and it's either uncompressed 
	// or compressed in a format that the mono channels can deal with
	bool bStereoCompressed = false;
	if(FAILED(IsStereoCompressed(pIRiffStream, pckMain, bStereoCompressed)))
	{
		return E_FAIL;
	}

	m_bCompressed = bStereoCompressed;
	m_pLeftChannel->m_bCompressed = false;
	m_pRightChannel->m_bCompressed = false;

	if(m_pWaveNode->IsInACollection() || !bStereoCompressed)
	{
		if(bStereoCompressed)
		{
			AfxMessageBox(IDS_ERR_INSERT_STEREO_COMPRESSED_WAVE_IN_COLLECTION);
			return E_FAIL;
		}

		IStream* pIStream = pIRiffStream->GetStream();
		ASSERT(pIStream);
		if(pIStream == NULL)
		{
			return E_OUTOFMEMORY;
		}

		// Remember where the stream pointer is positioned
		DWORD dwStreamPosition = StreamTell(pIStream);
		if(SUCCEEDED(m_pRightChannel->LoadHeader(pIRiffStream, pckMain)))
		{
			StreamSeek(pIStream, dwStreamPosition, STREAM_SEEK_SET);
			if(FAILED(m_pLeftChannel->LoadHeader(pIRiffStream, pckMain)))
			{
				pIStream->Release();
				return E_FAIL;
			}
		}
		else
		{
			pIStream->Release();
			return E_FAIL;
		}

		pIStream->Release();
		
		CopyMemory(&m_rWaveformat, &(m_pLeftChannel->m_rWaveformat), sizeof WAVEFORMATEX);
		GetTrueStereoFormat(m_rWaveformat);
		CopyMemory(&(m_OriginalWavObj.WaveFormatEx), &(m_pLeftChannel->m_rWaveformat), sizeof WAVEFORMATEX);
		GetTrueStereoFormat(m_OriginalWavObj.WaveFormatEx);
		m_OriginalWavObj.dwDataSize = m_pLeftChannel->GetUncompressedDataSize() * 2;
		m_dwDataSize = m_pLeftChannel->GetDataSize() * 2;

		// Sync up the info from the loaded mono waves
		SyncWaveProperties(true);
	}
	else
	{
		if(FAILED(CWave::LoadHeader(pIRiffStream, pckMain)))
		{
			return E_FAIL;
		}
	}

	// Update the DirestSoundHeader Stream
	if(SUCCEEDED(UpdateHeaderStream()))
	{
		m_pWaveNode->ReloadDirectSoundWave();
		NotifyWaveChange(false);
	}
	
	return S_OK;
}


HRESULT CStereoWave::LoadData(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain)
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

	ASSERT(m_pLeftChannel);
	if(m_pLeftChannel == NULL)
	{
		return E_FAIL;
	}

	ASSERT(m_pRightChannel);
	if(m_pRightChannel == NULL)
	{
		return E_FAIL;
	}

	// This is a wave in a DLS collection and it's either uncompressed 
	// or compressed in a format that the mono channels can deal with
	bool bStereoCompressed = false;
	if(FAILED(IsStereoCompressed(pIRiffStream, pckMain, bStereoCompressed)))
	{
		return E_FAIL;
	}

	if(m_pWaveNode->IsInACollection() || !bStereoCompressed)
	{
		// Load the data
		BYTE* pbData = NULL;
		DWORD cb = 0;
		if(FAILED(GetData(0, m_dwWaveLength, &pbData, cb)))
		{
			delete[] pbData;
			return E_FAIL;
		}

		// Set the data for the channels
		DMUSP_WAVEOBJECT stereoWave;
		stereoWave.dwDataSize = m_dwDataSize;
		stereoWave.pbData = pbData;
		stereoWave.WaveFormatEx = m_rWaveformat;
		stereoWave.pbExtractWaveFormatData = NULL;
		
		if(FAILED(m_pRightChannel->SetUncompressedData(stereoWave)))
		{
			delete[]  pbData;
			return E_FAIL;
		}

		if(FAILED(m_pLeftChannel->SetUncompressedData(stereoWave)))
		{
			delete[]  pbData;
			return E_FAIL;
		}

		if(FAILED(InterleaveChannelsForStereoData()))
		{
			delete[]  pbData;
			return E_FAIL;
		}

		m_OriginalWavObj.dwDataSize = m_pLeftChannel->GetUncompressedDataSize() + m_pRightChannel->GetUncompressedDataSize();
	}
	else
	{
		HRESULT hr = E_FAIL;
		MMCKINFO ck;
		ck.ckid = FOURCC_DATA;

		if(pIRiffStream->Descend(&ck, pckMain, MMIO_FINDCHUNK) == 0)
		{
			BYTE* pbData = m_OriginalWavObj.pbData;
			if(m_bCompressed && m_bLoadingRuntime)
			{
				pbData = m_CompressedWavObj.pbData;
			}

			if(FAILED(SafeAllocBuffer(&pbData, ck.cksize)))
			{
				return E_OUTOFMEMORY;
			}

			IStream* pIStream = pIRiffStream->GetStream();
			ASSERT(pIStream);
			if(pIStream == NULL)
			{
				return E_OUTOFMEMORY;
			}

			if(FAILED(CWave::ReadData(pIStream, pckMain, 0, ck.cksize, pbData)))
			{
				delete[] pbData;
				pIStream->Release();
				return E_FAIL;
			}

			pIStream->Release();

			if(m_bLoadingRuntime)
			{
				if(FAILED(UpdateDecompressedWave(&m_OriginalWavObj.WaveFormatEx)))
				{
					return E_FAIL;
				}
			}
			else
			{
				// Set the uncompressed data for the channels first
				if(FAILED(m_pLeftChannel->SetUncompressedData(m_OriginalWavObj)))
					return E_FAIL;
				if(FAILED(m_pRightChannel->SetUncompressedData(m_OriginalWavObj)))
					return E_FAIL;
				
				// Now compress and update the compressed data
				WAVEFORMATEX wfxPCM;
				GetTrueStereoFormat(wfxPCM);
				DWORD dwDataSize = GetUncompressedDataSize();
				
				BYTE* pwfxDst = NULL;
				if(FAILED(SafeAllocBuffer(&pwfxDst, sizeof WAVEFORMATEX + m_CompressedWavObj.WaveFormatEx.cbSize)))
				{
					return E_OUTOFMEMORY;
				}

				CopyMemory(pwfxDst, &m_CompressedWavObj.WaveFormatEx, sizeof WAVEFORMATEX);
				CopyMemory(pwfxDst + sizeof WAVEFORMATEX, m_CompressedWavObj.pbExtractWaveFormatData, m_CompressedWavObj.WaveFormatEx.cbSize);

				if(SUCCEEDED(ConvertStereoData(m_OriginalWavObj, ((WAVEFORMATEX*)pwfxDst), m_CompressedWavObj, true)))
				{
					m_bCompressed = true;
					if(FAILED(UpdateDecompressedWave(&wfxPCM)))
					{
						// Need to put up a messagebox
						SwitchCompression(false);
					}

					delete[] pwfxDst;
					return S_OK;
				}
				
				delete[] pwfxDst;
			}
		}
	}

	return S_OK;
}

HRESULT CStereoWave::ReadWVST(IStream* pIStream, MMCKINFO* pckMain)
{
	return S_OK;
}


HRESULT CStereoWave::IsStereoCompressed(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain, bool& bStereoCompressed)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pIRiffStream);
	if(pIRiffStream == NULL)
		return E_POINTER;

	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream == NULL)
		return E_FAIL;

	// Remember where the stream pointer is positioned
	DWORD dwStreamPosition = StreamTell(pIStream);

	HRESULT hr = S_OK;
	DWORD cSize = 0;
	MMCKINFO ck;
	ck.ckid = mmioFOURCC('w','a','v','u');
	if(pIRiffStream->Descend(&ck, pckMain, MMIO_FINDCHUNK) == 0) 
	{
		DWORD cb = 0;

		// Read the flag which indiactes Runtime or Designtime File
		hr = pIStream->Read(&m_bLoadingRuntime, sizeof bool, &cb);
		if(FAILED(hr) || cb != sizeof bool)
		{
			pIStream->Release();
			return hr;
		}


		// Read the flag that indicates compression
		hr = pIStream->Read(&m_bCompressed, sizeof bool, &cb);
		if(FAILED(hr) || cb != sizeof bool)
		{
			pIStream->Release();
			return hr;
		}
		
		// Read the WAVEFORMATEX data for the compressed wave
		WAVEFORMATEX stereoCompressionFormat;
		hr = pIStream->Read((LPSTR) &(stereoCompressionFormat), sizeof WAVEFORMATEX, &cb);	
		if (FAILED(hr) || cb != sizeof WAVEFORMATEX)
		{
			m_bCompressed = false;
			return E_FAIL;
		}

		BYTE* pbExtraInfo = NULL;
		if(stereoCompressionFormat.cbSize)
		{
			if(FAILED(SafeAllocBuffer(&pbExtraInfo, stereoCompressionFormat.cbSize)))
			{
				return E_OUTOFMEMORY;
			}

			if(FAILED(pIStream->Read(pbExtraInfo, stereoCompressionFormat.cbSize, &cb)))
			{
				delete[] pbExtraInfo;
			}
		}

		// We keep the compression info in designtime files / decompression info in runtime files
		if(m_bCompressed)
		{
			if(!m_bLoadingRuntime && stereoCompressionFormat.nChannels == 2)
			{
				bStereoCompressed = true;
				if(m_CompressedWavObj.pbExtractWaveFormatData)
				{
					delete[] m_CompressedWavObj.pbExtractWaveFormatData;
					m_CompressedWavObj.pbExtractWaveFormatData = NULL;
				}

				CopyMemory(&m_CompressedWavObj.WaveFormatEx, &stereoCompressionFormat, sizeof(WAVEFORMATEX));
				m_CompressedWavObj.pbExtractWaveFormatData = pbExtraInfo;
			}
			else if(m_bLoadingRuntime && stereoCompressionFormat.nChannels == 2)
			{
				bStereoCompressed = true;

				// This is really a PCM format that we want to decompress to
				CopyMemory(&m_OriginalWavObj.WaveFormatEx, &stereoCompressionFormat, sizeof(WAVEFORMATEX));
			}
		}
	}
	else
	{
		ck.ckid = mmioFOURCC('f','m','t',' ');
		StreamSeek(pIStream, dwStreamPosition, STREAM_SEEK_SET);
		if(pIRiffStream->Descend(&ck, pckMain, MMIO_FINDCHUNK) == 0)
		{
			WAVEFORMATEX wfxTemp;
			if(ck.cksize < sizeof(WAVEFORMATEX))
			{
				cSize = ck.cksize;
			}
			else
			{
				cSize = sizeof(WAVEFORMATEX);
			}

			DWORD cb = 0;
			hr = pIStream->Read(&wfxTemp, cSize, &cb);
			if(FAILED(hr) || cb != cSize)
			{
				pIStream->Release();
				pIRiffStream->Ascend(&ck, 0);
				return E_FAIL;
			}

			if(wfxTemp.wFormatTag != WAVE_FORMAT_PCM)
			{
				m_bLoadingRuntime = true;
				bStereoCompressed = true;

				CopyMemory(&m_CompressedWavObj.WaveFormatEx, &wfxTemp, sizeof(WAVEFORMATEX));
				
				if(m_CompressedWavObj.pbExtractWaveFormatData)
				{
					delete[] m_CompressedWavObj.pbExtractWaveFormatData;
					m_CompressedWavObj.pbExtractWaveFormatData = NULL;
				}

				if(FAILED(SafeAllocBuffer(&m_CompressedWavObj.pbExtractWaveFormatData, wfxTemp.cbSize)))
				{
					pIStream->Release();
					pIRiffStream->Ascend(&ck, 0);
					return E_OUTOFMEMORY;
				}

				if(FAILED(pIStream->Read(m_CompressedWavObj.pbExtractWaveFormatData, wfxTemp.cbSize, &cb)))
				{
					delete[] m_CompressedWavObj.pbExtractWaveFormatData;
					pIStream->Release();
					pIRiffStream->Ascend(&ck, 0);
					return E_FAIL;
				}
			}
			else
			{
				bStereoCompressed = false;
			}
		}
	}

	StreamSeek(pIStream, dwStreamPosition, STREAM_SEEK_SET );
	pIStream->Release();

	return hr;
}


HRESULT CStereoWave::LoadCompressedStereoData(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pIRiffStream);
	if(pIRiffStream == NULL)
		return E_POINTER;

	bool bReadWSMPL = false;
	bool bReadRSMPL = false;

	MMCKINFO ck;
	ck.ckid = 0;
	ck.fccType = 0;
	while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
		switch (ck.ckid) 
        {
			case mmioFOURCC('f','m','t',' '):
			{
				if(FAILED(ReadFormatChunk(pIRiffStream)))
				{
					return E_FAIL;
				}
			
				break;
			}

			case FOURCC_WSMP:
			{
				if(FAILED(ReadWSMPLChunk(pIRiffStream, ck.cksize)))
				{
					return E_FAIL;
				}

				bReadWSMPL = true;
				break;
			}

			case mmioFOURCC('s','m','p','l'):
			{
				if(FAILED(ReadRSMPLChunk(pIRiffStream, ck.cksize)))
				{
					return E_FAIL;	
				}
				
				bReadRSMPL = true;
				break;
			}

			case mmioFOURCC('d','a','t','a'):
			{
				HRESULT hr = E_FAIL;
				BYTE* pbDataBuffer = NULL;
				if(m_bLoadingRuntime && m_bCompressed)
				{
					m_CompressedWavObj.pbData = new BYTE[ck.cksize];
					pbDataBuffer = m_CompressedWavObj.pbData;
					m_CompressedWavObj.dwDataSize = ck.cksize;
				}
				else if(!m_bCompressed || !m_bLoadingRuntime)
				{
					m_OriginalWavObj.pbData = new BYTE[ck.cksize];
					pbDataBuffer = m_OriginalWavObj.pbData;
					m_OriginalWavObj.dwDataSize = ck.cksize;
				}

				if(pbDataBuffer == NULL)
				{
					return E_OUTOFMEMORY;
				}

				IStream* pIStream = pIRiffStream->GetStream();
				ASSERT(pIStream);
				if(pIStream == NULL)
					return E_FAIL;
								
				DWORD cb = 0;
				hr = pIStream->Read(pbDataBuffer, ck.cksize, &cb);
				if(FAILED(hr) || cb != ck.cksize)
				{
					pIStream->Release();
					return E_FAIL;
				}

				pIStream->Release();			

				break;
			}
			
			case mmioFOURCC('f','a','c','t'):
            case FOURCC_EDIT: 
			{
                break;
			}

			case FOURCC_LIST:
			{
				switch (ck.fccType)
				{
					case mmioFOURCC('I','N','F','O') :
					{
						m_Info.Load(pIRiffStream, &ck);
						break;
					}
					default:
					{
						// Compensation for fccType field : 
						// Descend puts the stream pointer after the fccType field 
						// but the size is still the old size so we need to adjust that
						ck.cksize -= sizeof DWORD; 
						m_UnknownChunk.Load(pIRiffStream, &ck, TRUE);
						
						// Reset the size so Ascend call goes through  properly
						ck.cksize += sizeof DWORD; 
						break;
					}
				}

				break;
			}
			
			default:
			{
                m_UnknownChunk.Load(pIRiffStream, &ck, FALSE);
                break;
			}
		}
		
		pIRiffStream->Ascend( &ck, 0 );
		ck.ckid = 0;
		ck.fccType = 0;
	}

	// Always give precedence to the legacy RSMPL loop structure
	// Otherwise we might end up ignoring changes made to the wave 
	// files in SoundForge etc. which also have WSMPL chunks in them
	if (bReadRSMPL)
    {
		// The RSMPL structure field dwMIDIPitchFraction keeps a +ve value 
		// that is the fraction of a semitone above the untiy note
		// WSMP on the other hand let's you have a fine tune value between 
		// -12 to +12 semitones. Since we always give RSMPL chunk the precedence 
		// copying the RSMPL values into WSMPL struct will make the unitynote 
		// and fine tune values jump which results in some problems...
		// So we check if the values are basically the same and if they are we use
		// the WSMPL values instead of RSMPL. Easy enough?
		short sUnityNote = m_rWSMP.usUnityNote;
		short sFineTune = m_rWSMP.sFineTune;
		CopySMPLToWSMP();
		if(IsUnityNoteAndFractionSame(m_rRSMPL, sUnityNote, sFineTune))
		{
			m_rWSMP.usUnityNote = sUnityNote;
			m_rWSMP.sFineTune = sFineTune;
		}

	}
	else
	{
		CopyWSMPToSMPL();
	}

	// Set the loop flag
	if(m_rWSMP.cSampleLoops > 0)
		m_bPlayLooped = true;
	else
		m_bPlayLooped = false;

	// Validate the loop values...
	// These might have been set to illegal values outside Producer
	if(m_rWLOOP.ulStart > m_dwWaveLength)
		m_rWLOOP.ulStart = 0;
	if(m_rWLOOP.ulStart + m_rWLOOP.ulLength > m_dwWaveLength)
		m_rWLOOP.ulLength = m_dwWaveLength - m_rWLOOP.ulStart;


	SyncWaveProperties(false);

    return S_OK;
}



HRESULT CStereoWave::Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain, bool bLoadRightChannel)
{
	ASSERT(m_pWaveNode);
	if(m_pWaveNode == NULL)
	{
		return E_UNEXPECTED;
	}

	ASSERT(m_pLeftChannel);
	if(m_pLeftChannel == NULL)
		return E_FAIL;
	ASSERT(m_pRightChannel);
	if(m_pRightChannel == NULL)
		return E_FAIL;

	HRESULT hr = E_FAIL;

	if(bLoadRightChannel)
	{
		hr = m_pRightChannel->Load(pIRiffStream, pckMain);
	}
	else
	{
		hr = m_pLeftChannel->Load(pIRiffStream, pckMain);
	}

    m_OriginalWavObj.dwDataSize = m_pLeftChannel->GetUncompressedDataSize() * 2;

	// Sync up the info from the loaded mono waves
	SyncWaveProperties(true);

	return hr;
}

HRESULT CStereoWave::UpdateWave()
{
	ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);

	SyncWaveProperties(true);

	if(FAILED(m_pLeftChannel->UpdateWave()))
		return E_FAIL;
	
	if(FAILED(m_pRightChannel->UpdateWave()))
		return E_FAIL;

	return S_OK;
}

HRESULT	CStereoWave::UpdateDecompressedWave(const WAVEFORMATEX* pwfxDst)
{
	ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);

	if(m_pCollection)
	{
		if(FAILED(m_pLeftChannel->UpdateDecompressedWave(pwfxDst)))
			return E_FAIL;
		
		if(FAILED(m_pRightChannel->UpdateDecompressedWave(pwfxDst)))
			return E_FAIL;

		SyncWaveProperties(true);
		return S_OK;
	}
	else if(m_bCompressed)
	{
		if(SUCCEEDED(ConvertStereoData(m_CompressedWavObj, pwfxDst, m_DecompressedWavObj, false)))
		{
			if(FAILED(UpdateChannelsForStereoCompression()))
			{
				SwitchCompression(false);
				return E_FAIL;
			}

			SyncWaveProperties(true);
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


HRESULT	CStereoWave::SwitchCompression(bool bSwitch)
{
	ASSERT(m_pLeftChannel);
    if(m_pLeftChannel == NULL)
    {
        return E_UNEXPECTED;
    }

	ASSERT(m_pRightChannel);
    if(m_pRightChannel == NULL)
    {
        return E_UNEXPECTED;
    }
	
	// no work to do if already pointing to appropriate wave
	if (m_bCompressed == bSwitch)
		return S_OK;

	if(m_pCollection || !bSwitch)
	{
		m_pLeftChannel->SetWaveBufferToCompressed(bSwitch);
		m_pRightChannel->SetWaveBufferToCompressed(bSwitch);
	}
	else if(bSwitch)
	{
		if(FAILED(UpdateChannelsForStereoCompression()))
			return E_FAIL;
	}

    m_bCompressed = bSwitch;

	SyncWaveProperties(true);

    // Update the header stream
	if(SUCCEEDED(UpdateHeaderStream()))
	{
		m_pWaveNode->ReloadDirectSoundWave();
	}

    RefreshDrawBuffer();

	CWaveCtrl* pWaveEditor = m_pWaveNode->GetWaveEditor();
	if(pWaveEditor)
	{
		pWaveEditor->InvalidateControl(); // Repaint the wave with new values
	}

	return S_OK;
}


HRESULT	CStereoWave::OnWaveBufferUpdated()
{
	ASSERT(m_pLeftChannel);
	if(m_pLeftChannel == NULL)
		return E_FAIL;

	ASSERT(m_pRightChannel);
	if(m_pRightChannel == NULL)
		return E_FAIL;

	if(SUCCEEDED(m_pLeftChannel->OnWaveBufferUpdated()))
	{
		if(SUCCEEDED(m_pRightChannel->OnWaveBufferUpdated()))
		{
			m_dwDataSize = m_pLeftChannel->GetDataSize() * 2;
			if (!m_bCompressed)
				m_OriginalWavObj.dwDataSize = m_dwDataSize;

			SyncWaveProperties(true);
			UpdateDummyInstrument();
			
			if(FAILED(CWave::OnWaveBufferUpdated()))
			{
				TRACE("DirectSoundWave header update failed!");
				return E_FAIL;
			}

			return S_OK;
		}
	}

	return E_FAIL;
}

CMonoWave* CStereoWave::GetLeftChannel()
{
	return m_pLeftChannel;
}

CMonoWave* CStereoWave::GetRightChannel()
{
	return m_pRightChannel;
}

HRESULT CStereoWave::Compress(const WAVEFORMATEX* pwfxDst, bool bInUndoRedo)
{
	ASSERT(m_pLeftChannel);
	if(m_pLeftChannel == NULL)
		return E_FAIL;

	ASSERT(m_pRightChannel);
	if(m_pRightChannel == NULL)
		return E_FAIL;

	if(m_pCollection)
	{
		if(SUCCEEDED(m_pLeftChannel->Compress(pwfxDst, bInUndoRedo)))
		{
			if(SUCCEEDED(m_pRightChannel->Compress(pwfxDst, bInUndoRedo)))
			{
				SyncWaveProperties(true);
				return S_OK;
			}
		}
	}
	else
	{
		if(FAILED(CompressStereoData(pwfxDst, bInUndoRedo)))
			return E_FAIL;

		return S_OK;
	}

	return E_FAIL;
}


HRESULT CStereoWave::CompressStereoData(const WAVEFORMATEX* pwfxDest, bool bInUndoRedo)
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
    DWORD dwWaveLength = (nBytesPerSample == 1) ? (dwDataSize / 2) : ((dwDataSize >> 1) / 2);

	// Remember the old wavelength
	DWORD dwOldLength = dwWaveLength;

	// We have to compress the wave in chunks
	DWORD dwSamplesCompressed = 0;
	DWORD dwSamplesToCompress = dwWaveLength * 2;
	
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

		DWORD cb = 0;
		if(FAILED(GetData(dwSamplesCompressed, dwLength, &pbData, cb, true)))
		{
			return E_FAIL;
		}

		// Now compress the data
		DWORD dwSourceSize = dwLength * nBytesPerSample * 2;
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
		dwSamplesToCompress -= m_dwWaveLength - dwSamplesCompressed;

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
	m_dwWaveLength = (dwDecompressedSize / nBytesPerSample) / 2;
	
	// Mark the wave as compressed...
	m_bCompressed = true;

	// Mark the channels for compression
	m_pLeftChannel->m_bCompressed = true;
	m_pRightChannel->m_bCompressed = true;

	// Update the loop points if it's looped...
	if(m_bPlayLooped)
	{
		LONG lDifference = m_dwWaveLength - dwOldLength;
		m_rWLOOP.ulStart += lDifference;
		m_rWLOOP.ulStart = max(0, ((long)m_rWLOOP.ulStart));
		m_rWLOOP.ulStart = min(((long)m_rWLOOP.ulStart), (long)m_dwWaveLength);
		m_rWLOOP.ulLength = min(m_rWLOOP.ulLength, m_dwWaveLength - m_rWLOOP.ulStart);
		
		m_rRLOOP.dwStart = m_rWLOOP.ulStart;
		m_rRLOOP.dwEnd = m_rWLOOP.ulStart + m_rWLOOP.ulLength - 1;
	}

	if(FAILED(UpdateChannelsForStereoCompression()))
	{
		return E_FAIL;
	}

	return S_OK;
}




HRESULT CStereoWave::Download(IDirectMusicPortDownload* pIDMPortDownload)
{
	// Validate the passed port and the waves for both the channels
	ASSERT(pIDMPortDownload);
	if(pIDMPortDownload == NULL)
		return E_POINTER;

	ASSERT(m_pLeftChannel);
	if(m_pLeftChannel == NULL)
		return E_FAIL;

	ASSERT(m_pRightChannel);
	if(m_pRightChannel == NULL)
		return E_FAIL;

	if(FAILED(m_pLeftChannel->Download(pIDMPortDownload)))
		return E_FAIL;

	if(FAILED(m_pRightChannel->Download(pIDMPortDownload)))
		return E_FAIL;

	if(m_pDummyInstrument)
	{
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
	}

	return S_OK;
}

void CStereoWave::UpdateLoopInRegions()
{
	ASSERT(m_pLeftChannel);
	if(m_pLeftChannel == NULL)
		return;
	
	ASSERT(m_pRightChannel);
	if(m_pRightChannel == NULL)
		return;

	m_pLeftChannel->UpdateLoopInRegions();
	m_pRightChannel->UpdateLoopInRegions();

	CWave::UpdateLoopInRegions();

	// Sync up the info from the loaded mono waves
	SyncWaveProperties(true);
}

HRESULT CStereoWave::DM_Init(IDirectMusicPortDownload* pIDMPortDownload)
{
	ASSERT(pIDMPortDownload);
	if(pIDMPortDownload == NULL)
	{ 
		return E_POINTER;
	}

	ASSERT(m_pLeftChannel);
	if(m_pLeftChannel == NULL)
	{
		return E_FAIL;
	}
	
	ASSERT(m_pRightChannel);
	if(m_pRightChannel == NULL)
	{
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;
	if(SUCCEEDED(hr = m_pLeftChannel->DM_Init(pIDMPortDownload)))
	{
		hr = m_pRightChannel->DM_Init(pIDMPortDownload);
	}

	return hr;
}

HRESULT CStereoWave::Unload(IDirectMusicPortDownload* pIDMPortDownload)
{
	ASSERT(pIDMPortDownload);
	if(pIDMPortDownload == NULL)
	{
		return E_POINTER;
	}

	ASSERT(m_pLeftChannel);
	if(m_pLeftChannel == NULL)
	{
		return E_FAIL;
	}
	
	ASSERT(m_pRightChannel);
	if(m_pRightChannel == NULL)
	{
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;
	if(SUCCEEDED(hr = m_pLeftChannel->Unload(pIDMPortDownload)))
	{
		hr = m_pRightChannel->Unload(pIDMPortDownload);
	}

	// Unload the dummy instrument first
	if(m_pDummyInstrument)
	{
		if(FAILED(m_pDummyInstrument->Unload(pIDMPortDownload)))
			return E_FAIL;
	}

	return hr;
}

HRESULT CStereoWave::SaveAs(IStream* pIStream, bool bPromptForFileName)
{
	// If we need a name call the base class saveas method
	if(bPromptForFileName)
	{
		return CWave::SaveAs(pIStream, bPromptForFileName);
	}

	if(pIStream)
	{
		return SaveAs(pIStream);
	}

	return E_POINTER;
}


// Saves the out as a true stereo wave file
HRESULT CStereoWave::SaveAs(IStream* pIStream)
{
	ASSERT(pIStream);
	if(pIStream == NULL)
		return E_POINTER;

	if(FAILED(Save(pIStream)))
	{
		return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CStereoWave::Save(IStream* pIStream)
{
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_POINTER;
	}

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


	IDMUSProdRIFFStream* pIRiffStream = NULL;
	if(FAILED(AllocRIFFStream(pIStream, &pIRiffStream)))
	{
		return E_FAIL;
	}

	bool bRuntime = false;
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

	// Create wave riff header
	m_dwHeaderOffset = GetFilePos(pIStream);

	MMCKINFO ckMain;
	ckMain.fccType = mmioFOURCC('W','A','V','E'); 
	if(FAILED(pIRiffStream->CreateChunk(&ckMain, MMIO_CREATERIFF)))
	{
		pIRiffStream->Release();
		return E_FAIL;
	}

	// Write the GUID chunk
	if(FAILED(CWave::WriteGUIDChunk(pIRiffStream)))
	{
		pIRiffStream->Release();
		return E_FAIL;
	}


	// Write the compression information
	if(m_bCompressed)
	{
		if(FAILED(WriteCompressionChunk(pIRiffStream, bRuntime)))
		{
			pIRiffStream->Release();
			return E_FAIL;
		}
	}

	// Save the 'wavh' chunk that keeps the streaming info
	MMCKINFO ck;
	ck.ckid = DMUS_FOURCC_WAVEHEADER_CHUNK;
	HRESULT hr = pIRiffStream->CreateChunk(&ck, 0);
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
	
	if (FAILED(pIRiffStream->Ascend(&ck, 0)))
	{
		pIStream->Release();
		return E_FAIL;
	}



	if(FAILED(WriteStereoFormat(pIRiffStream, bRuntime)))
	{
		pIRiffStream->Release();
		return E_FAIL;
	}

	if(!bRuntime || !m_bCompressed)
	{
		if(FAILED(WriteStereoData(pIRiffStream, bRuntime)))
		{
			pIRiffStream->Release();
			return E_FAIL;
		}
	}
	else
	{
		if(FAILED(WriteFACTChunk(pIRiffStream)))
		{
			pIRiffStream->Release();
			return E_FAIL;
		}

		if(FAILED(WriteCompressedData(pIRiffStream)))
		{
			pIRiffStream->Release();
			return E_FAIL;
		}
	}
	
	if(FAILED(WriteRSMPLChunk(pIRiffStream)))
	{
		pIRiffStream->Release();
		return E_FAIL;
	}

	if(FAILED(WriteWSMPLChunk(pIRiffStream)))
	{
		pIRiffStream->Release();
		return E_FAIL;
	}

	if(FAILED(m_Info.Save(pIRiffStream)))
	{
		pIRiffStream->Release();
		return E_FAIL;
	}

	pIRiffStream->Ascend(&ckMain, 0);
	pIRiffStream->Release();

	return S_OK;
}


HRESULT CStereoWave::WriteStereoFormat(IDMUSProdRIFFStream* pIRiffStream, bool bRuntime)
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
		return E_POINTER;
	}

	WAVEFORMATEX wfxPCM;
	WAVEFORMATEX* pStereoFormat = NULL;
	BYTE* pbExtraInfo = NULL;
	if(!m_bCompressed || !bRuntime)
	{
		GetTrueStereoFormat(wfxPCM);
		pStereoFormat = &wfxPCM;
	}
	else if(m_bCompressed && bRuntime)
	{
		pStereoFormat = &(m_CompressedWavObj.WaveFormatEx);
		pbExtraInfo = m_CompressedWavObj.pbExtractWaveFormatData;
	}
	
	// Create format chunk
	MMCKINFO ck;
	DWORD cb = 0;

	ck.ckid = mmioFOURCC('f','m','t',' ');
	if(FAILED(pIRiffStream->CreateChunk(&ck, 0)))
	{
		pIStream->Release();
		return E_FAIL;
	}

	// Write the WAVEFORMATEX
	HRESULT hr = pIStream->Write((LPSTR) pStereoFormat, sizeof WAVEFORMATEX, &cb);	
	if(FAILED(hr) || cb != sizeof WAVEFORMATEX)
	{
		pIStream->Release();
		return E_FAIL;
	}

	// Write the extra info
	hr = pIStream->Write(pbExtraInfo, pStereoFormat->cbSize, &cb);	
	if(FAILED(hr) || cb != pStereoFormat->cbSize)
	{
		pIStream->Release();
		return E_FAIL;
	}
	
	if(FAILED(pIRiffStream->Ascend(&ck, 0)))
	{
		pIStream->Release();
		return E_FAIL;
	}

	pIStream->Release();
	return S_OK;
}


HRESULT CStereoWave::WriteCompressionChunk(IDMUSProdRIFFStream* pIRiffStream, bool bRuntime)
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
		return E_POINTER;
	}

	// Compressed wave info chunk
	MMCKINFO ck;
	DWORD cb = 0;

	ck.ckid = mmioFOURCC('w','a','v','u'); 
	if(FAILED(pIRiffStream->CreateChunk(&ck, 0)))
	{
		pIStream->Release();
		return E_FAIL;
	}

	// Save the flag that marks whether this file is Runtime or Designtime
	HRESULT hr = pIStream->Write((LPSTR) &(bRuntime), sizeof bool, &cb);	
	if(FAILED(hr) || cb != sizeof bool)
	{
		pIStream->Release();
		return E_FAIL;
	}

	// Save the flag that marks compression
	hr = pIStream->Write((LPSTR) &(m_bCompressed), sizeof bool, &cb);	
	if(FAILED(hr) || cb != sizeof bool)
	{
		pIStream->Release();
		return E_FAIL;
	}

	// Save the compression format in the wavu chunk if this is not a runtime save
	if(m_bCompressed && bRuntime == false)
	{
		// Save the format header
		hr = pIStream->Write((LPSTR) &(m_CompressedWavObj.WaveFormatEx), sizeof WAVEFORMATEX, &cb);	
		if(FAILED(hr) || cb != sizeof WAVEFORMATEX)
		{
			pIStream->Release();
			return E_FAIL;
		}
		
		// Compressed waves might have extra information in the header
		if(m_CompressedWavObj.WaveFormatEx.cbSize > 0)
		{
			BYTE* pbExtraInfo = m_CompressedWavObj.pbExtractWaveFormatData;
			hr = pIStream->Write(pbExtraInfo, m_CompressedWavObj.WaveFormatEx.cbSize, &cb);
			if(FAILED(hr) || cb != m_CompressedWavObj.WaveFormatEx.cbSize)
			{
				pIStream->Release();
				return E_FAIL;
			}
		}
	}
	if(m_bCompressed && bRuntime)
	{
		WAVEFORMATEX wfxPCM;
		GetTrueStereoFormat(wfxPCM);

		hr = pIStream->Write((LPSTR) &wfxPCM, sizeof WAVEFORMATEX, &cb);	
		if (FAILED(hr) || cb != sizeof WAVEFORMATEX)
		{
			pIStream->Release();
			return E_FAIL;
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



HRESULT CStereoWave::WriteRSMPLChunk(IDMUSProdRIFFStream* pIRiffStream)
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
		return E_POINTER;
	}

	// Create RSMPL chunk
	MMCKINFO ck;
	DWORD cb = 0;

	ck.ckid = mmioFOURCC('s','m','p','l');
    if(FAILED(pIRiffStream->CreateChunk(&ck, 0)))
    {
		pIStream->Release();
        return E_FAIL;
    }

    DWORD dwSaveSize = sizeof RSMPL;
    if(m_rRSMPL.cSampleLoops > 0)
    {
        dwSaveSize += sizeof RLOOP;
        m_rRSMPL.cSampleLoops = 1;
    }

    HRESULT hr = pIStream->Write((LPSTR) &m_rRSMPL,dwSaveSize, &cb);
    if(FAILED(hr) || cb != dwSaveSize)
    {
		pIStream->Release();
        return E_FAIL;
    }

	if(FAILED(pIRiffStream->Ascend(&ck, 0)))
	{
		pIStream->Release();
		return E_FAIL;
	}

	pIStream->Release();
	return S_OK;

}

HRESULT CStereoWave::WriteWSMPLChunk(IDMUSProdRIFFStream* pIRiffStream)
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
		return E_POINTER;
	}

	// Create WSMPL chunk
	MMCKINFO ck;
	DWORD cb = 0;

	ck.ckid = FOURCC_WSMP;
    if(FAILED(pIRiffStream->CreateChunk(&ck, 0)))
    {
		pIStream->Release();
        return E_FAIL;
    }

    DWORD dwSaveSize = sizeof WSMPL;
    if(m_rWSMP.cSampleLoops > 0)
    {
        dwSaveSize += sizeof WLOOP;
        m_rWSMP.cSampleLoops = 1;
    }

    HRESULT hr = pIStream->Write((LPSTR) &m_rWSMP, dwSaveSize, &cb);
    if(FAILED(hr) || cb != dwSaveSize)
    {
		pIStream->Release();
        return E_FAIL;
    }

	if(FAILED(pIRiffStream->Ascend(&ck, 0)))
	{
		pIStream->Release();
		return E_FAIL;
	}

	pIStream->Release();
	return S_OK;

}


HRESULT CStereoWave::WriteStereoData(IDMUSProdRIFFStream* pIRiffStream, bool bRuntime)
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
		return E_POINTER;
	}

	ASSERT(m_pDataManager);
	if(m_pDataManager == NULL)
	{
		return E_FAIL;
	}

	// Save data
	MMCKINFO ck;
	DWORD cb = 0;

	m_dwDataOffset = GetFilePos(pIStream) + 8;
	
	ck.ckid = mmioFOURCC('d','a','t','a') ;
	if(FAILED(pIRiffStream->CreateChunk(&ck,0)))
	{
		pIStream->Release();
		return E_FAIL;
	}

	if(IsTrueStereo())
	{
		if(FAILED(m_pDataManager->SaveUncompressedDataToStream(pIStream, cb)))
		{
			pIStream->Release();
			return E_FAIL;
		}
	}
	else
	{
		// Intereleave the data from the channels and then write it to the stream
		if(FAILED(WriteInterleavedData(pIStream)))
		{
			pIStream->Release();
			return E_FAIL;
		}
	}

	HRESULT hr = pIRiffStream->Ascend(&ck, 0);
	if(FAILED(hr))
	{
		pIStream->Release();
		return E_FAIL;
	}

	pIStream->Release();
	return S_OK;

}


HRESULT CStereoWave::WriteInterleavedData(IStream* pIStream)
{
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_POINTER;
	}

	ASSERT(m_pLeftChannel);
	if(m_pLeftChannel == NULL)
	{
		return E_UNEXPECTED;
	}

	ASSERT(m_pRightChannel);
	if(m_pRightChannel == NULL)
	{
		return E_UNEXPECTED;
	}

	DWORD dwSamplesToWrite = m_dwWaveLength;
	DWORD dwSamplesWritten = 0;
	UINT nBytesPerSample = m_rWaveformat.wBitsPerSample / 8;

	
	while(dwSamplesWritten < m_dwWaveLength)
	{
		if(dwSamplesToWrite > WAVE_DATA_BUFFER_SIZE * 2)
		{
			dwSamplesToWrite = WAVE_DATA_BUFFER_SIZE;
		}
		
		DWORD byRead = 0;
		BYTE* pbLeft = NULL;
		if(FAILED(m_pLeftChannel->GetData(dwSamplesWritten, dwSamplesToWrite, &pbLeft, byRead)))
		{
			return E_FAIL;
		}

		byRead = 0;
		BYTE* pbRight = NULL;
		if(FAILED(m_pRightChannel->GetData(dwSamplesWritten, dwSamplesToWrite, &pbRight, byRead)))
		{
			return E_FAIL;
		}

		// Interleave the data
		BYTE* pbStereo = NULL;
		DWORD dwStereoSize = dwSamplesToWrite * 2 * nBytesPerSample;
		if(FAILED(SafeAllocBuffer(&pbStereo, dwStereoSize)))
		{
			return E_OUTOFMEMORY;
		}

		if(FAILED(InterleaveBuffersForStereoData(pbLeft, pbRight, pbStereo, m_rWaveformat, dwStereoSize)))
		{
			return E_FAIL;
		}

		// Write it to the stream
		DWORD cbWritten = 0;
		if(FAILED(pIStream->Write(pbStereo, dwStereoSize, &cbWritten)))
		{
			return E_FAIL;
		}

		if(cbWritten != dwStereoSize)
		{
			return E_FAIL;
		}

		delete[] pbStereo;
		delete[] pbLeft;
		delete[] pbRight;

		dwSamplesWritten += dwSamplesToWrite;
		dwSamplesToWrite = m_dwWaveLength - dwSamplesWritten;
	}

	return S_OK;
}


HRESULT CStereoWave::WriteFACTChunk(IDMUSProdRIFFStream* pIRiffStream)
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
		return E_POINTER;
	}

	MMCKINFO ck;
	ck.ckid = mmioFOURCC('f','a','c','t');
	if(FAILED(pIRiffStream->CreateChunk(&ck,0)))
	{
		pIStream->Release();
		return E_FAIL;
	}

	// Write the number of original samples (before compression)
	int nSampleSize = m_OriginalWavObj.WaveFormatEx.wBitsPerSample / 8;
	DWORD dwSamples = m_OriginalWavObj.dwDataSize / nSampleSize;
	DWORD dwLength = dwSamples / 2;
	
	DWORD cb = 0;
	HRESULT hr = pIStream->Write((LPSTR) &(dwLength), sizeof(DWORD), &cb);
	if(FAILED(hr) || cb != sizeof(DWORD))
	{
		pIStream->Release();
		return E_FAIL;
	}

	hr = pIRiffStream->Ascend(&ck, 0);
	if(FAILED(hr))
	{
		pIStream->Release();
		return E_FAIL;
	}

	pIStream->Release();
	return S_OK;
}


HRESULT CStereoWave::WriteCompressedData(IDMUSProdRIFFStream* pIRiffStream)
{
	ASSERT(pIRiffStream);
	if(pIRiffStream == NULL)
	{
		return E_POINTER;
	}

	ASSERT(m_pDataManager);
	if(m_pDataManager == NULL)
	{
		return E_FAIL;
	}

	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_OUTOFMEMORY;
	}

	// We position the data offset just after the chunk header
	m_dwDataOffset = GetFilePos(pIStream) + 8;

	MMCKINFO ck;
	ck.ckid = mmioFOURCC('d','a','t','a') ;
	HRESULT hr = pIRiffStream->CreateChunk(&ck,0);
	if (FAILED(hr))
	{
		pIStream->Release();
		return hr;
	}

	UINT nExtraBytes = m_CompressedWavObj.WaveFormatEx.cbSize;
	WAVEFORMATEX* pwfxDest = NULL;
	if(FAILED(SafeAllocBuffer((BYTE**)&pwfxDest, sizeof(WAVEFORMATEX) + nExtraBytes)))
	{
		return E_OUTOFMEMORY;
	}

	CopyMemory((BYTE*)pwfxDest, &m_CompressedWavObj.WaveFormatEx, sizeof(WAVEFORMATEX));
	CopyMemory((BYTE*)pwfxDest + sizeof(WAVEFORMATEX), m_CompressedWavObj.pbExtractWaveFormatData, nExtraBytes); 


	int nBytesPerSample = m_rWaveformat.wBitsPerSample / 8;

	// We have to compress the wave in chunks
	DWORD dwSamplesCompressed = 0;
	DWORD dwSourceLength = (m_OriginalWavObj.dwDataSize / nBytesPerSample) / 2;
	DWORD dwSamplesToCompress = dwSourceLength * 2;
	
	DWORD dwCompressedSize = 0;
	DWORD dwDecompressedSize = 0;

	// Get data in chunks of WAVE_DATA_BUFFER_SIZE 
	while(dwSamplesCompressed < dwSourceLength)
	{
		BYTE* pbData = NULL;
		DWORD dwLength = dwSourceLength; //WAVE_DATA_BUFFER_SIZE;
		dwSamplesToCompress = dwSourceLength - dwSamplesCompressed;
		
		// Take the big slice for the last chunk....
		// The conversion might fail if we take very small amount of data
		/*if(dwSamplesToCompress <= WAVE_DATA_BUFFER_SIZE * 2)
		{
			dwLength = dwSamplesToCompress;
		}*/

		
		// Turn off the compression temporarily to fool the GetChannelData
		// into getting us the uncompressed data for compression else we end
		// up compressing the decompressed wave data...
		m_bCompressed = false;

		DWORD cb = 0;
		if(FAILED(GetData(dwSamplesCompressed, dwLength, &pbData, cb)))
		{
			m_bCompressed = true;
			delete[] pwfxDest;
			pIStream->Release();
			return E_FAIL;
		}

		// Turn compression back on...
		m_bCompressed = true;

		// Now compress the data
		DWORD dwSourceSize = dwLength * nBytesPerSample * 2;
		BYTE* pbCompressed = NULL;
		DWORD dwCompressed = 0;
		if(FAILED(ConvertWave(&m_rWaveformat, pwfxDest, pbData, dwSourceSize, &pbCompressed, dwCompressed)))
		{
			delete[] pwfxDest;
			pIStream->Release();
			delete[] pbData;
			return E_FAIL;
		}

		dwCompressedSize += dwCompressed;
		
		// Update the number of samples successfully compressed
		dwSamplesCompressed += dwLength;

		// Write the data to the stream
		if(FAILED(pIStream->Write(pbCompressed, dwCompressed, &cb)))
		{
			delete[] pwfxDest;
			pIStream->Release();
			delete[] pbData;
			return E_FAIL;
		}

		delete[] pbData;
		delete[] pbCompressed;
	}


	delete[] pwfxDest;
	if(FAILED(pIRiffStream->Ascend(&ck, 0)))
	{
		pIStream->Release();
		return E_FAIL;
	}

	pIStream->Release();
	return S_OK;
}




HRESULT CStereoWave::Save(IDMUSProdRIFFStream* pIRiffStream, UINT wType, BOOL fFullSave)
{
	ASSERT(pIRiffStream);
	if(pIRiffStream == NULL)
	{
		return E_POINTER;
	}

	ASSERT(m_pLeftChannel);
	if(m_pLeftChannel == NULL)
	{
		return E_FAIL;
	}
	
	ASSERT(m_pRightChannel);
	if(m_pRightChannel == NULL)
	{
		return E_FAIL;
	}

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_FAIL;
	}

    bool bRuntime = false;
    IDMUSProdPersistInfo* pIJazzPersistInfo = NULL;
	if(SUCCEEDED(pIStream->QueryInterface(IID_IDMUSProdPersistInfo,(void**) &pIJazzPersistInfo)))
	{
		DMUSProdStreamInfo StreamInfo;
		FileType ftSaveType;

		pIJazzPersistInfo->GetStreamInfo(&StreamInfo);
		ftSaveType = StreamInfo.ftFileType;

		if (ftSaveType == FT_RUNTIME)
        {
			bRuntime = true;
        }
		
		pIJazzPersistInfo->Release();		
	}


	HRESULT hr = E_FAIL;
	if(m_pCollection)
	{
        // Ask the data mnager to update the uncomrpessed deltas
        if(!m_bCompressed)
        {
            if(FAILED(m_pDataManager->UpdateUncompressedDeltas()))
            {
                return E_FAIL;
            }
        }

		if(SUCCEEDED(hr = m_pLeftChannel->Save(pIRiffStream, wType, fFullSave)))
		{
			hr = m_pRightChannel->Save(pIRiffStream, wType, fFullSave);
		}
	}
	else
	{
		hr = Save(pIStream);
	}

    pIStream->Release();
	return hr;
}

void CStereoWave::RefreshDrawBuffer()
{
    ASSERT(m_pLeftChannel);
	if(m_pLeftChannel == NULL)
	{
		return;
	}
	
	ASSERT(m_pRightChannel);
	if(m_pRightChannel == NULL)
	{
		return;
	}

    m_pLeftChannel->RefreshDrawBuffer();
    m_pRightChannel->RefreshDrawBuffer();
}

void CStereoWave::OnDraw(CDC* pDC, const CRect& rcClient)
{
	ASSERT(m_pWaveNode);
	if(m_pWaveNode == NULL)
	{
		return;
	}

	ASSERT(m_pLeftChannel);
	if(m_pLeftChannel == NULL)
	{
		return;
	}
	
	ASSERT(m_pRightChannel);
	if(m_pRightChannel == NULL)
	{
		return;
	}

	ASSERT(pDC);
	if(pDC == NULL)
	{
		return;
	}

	// Divide the client rect horizontally in half...top for the left channel, bottom for the right
	CRect rcLeftChannel;
	rcLeftChannel.top = rcClient.top;
	rcLeftChannel.bottom = rcClient.top + ((rcClient.bottom - rcClient.top) / 2);
	rcLeftChannel.left = rcClient.left;
	rcLeftChannel.right = rcClient.right;

	CRect rcRightChannel;
	rcRightChannel.top = rcClient.bottom - ((rcClient.bottom - rcClient.top) / 2) ;
	rcRightChannel.left = rcClient.left;
	rcRightChannel.bottom = rcClient.bottom;
	rcRightChannel.right = rcClient.right;

	// Draw the channel divinding line
	CPen* pOldPen = NULL;
	CPen penDividingLine(PS_SOLID, 0, RGB(0, 176, 0));
	pOldPen = pDC->SelectObject(&penDividingLine);
	pDC->MoveTo(rcClient.left, rcLeftChannel.bottom);
	pDC->LineTo(rcClient.right, rcLeftChannel.bottom);
	if(pOldPen)
	{
		pDC->SelectObject(pOldPen);
	}


	m_pLeftChannel->OnDraw(pDC, rcLeftChannel);
	m_pRightChannel->OnDraw(pDC, rcRightChannel);
	
	return;
}

void CStereoWave::OnDraw(CDC* pDC, const CRect& rcClient, const long lOffset, DWORD& dwStartSample, DWORD dwLength, float fSamplesPerPixel, const WaveDrawParams* pDrawParams, const WaveTimelineInfoParams* pInfoParams)
{
	ASSERT(m_pLeftChannel);
	if(m_pLeftChannel == NULL)
	{
		return;
	}
	
	ASSERT(m_pRightChannel);
	if(m_pRightChannel == NULL)
	{
		return;
	}

	ASSERT(pDC);
	if(pDC == NULL)
	{
		return;
	}

	// Divide the client rect horizontally in half...top for the left channel, bottom for the right
	CRect rcLeftChannel;
	rcLeftChannel.top = rcClient.top;
	rcLeftChannel.bottom = rcClient.top + ((rcClient.bottom - rcClient.top) / 2);
	rcLeftChannel.left = rcClient.left;
	rcLeftChannel.right = rcClient.right;

	CRect rcRightChannel;
	rcRightChannel.top = rcClient.bottom - ((rcClient.bottom - rcClient.top) / 2) ;
	rcRightChannel.left = rcClient.left;
	rcRightChannel.bottom = rcClient.bottom;
	rcRightChannel.right = rcClient.right;

	// Draw the channel divinding line
	CPen* pOldPen = NULL;
	CPen penDividingLine(PS_SOLID, 0, RGB(0, 176, 0));
	pOldPen = pDC->SelectObject(&penDividingLine);
	pDC->MoveTo(rcClient.left, rcLeftChannel.bottom);
	pDC->LineTo(rcClient.right, rcLeftChannel.bottom);
	if(pOldPen)
	{
		pDC->SelectObject(pOldPen);
	}

	// Make a copy of the reference so both channels draw at the same position
	DWORD dwFirstSample = dwStartSample;
	m_pLeftChannel->OnDraw(pDC, rcLeftChannel, lOffset, dwFirstSample, dwLength, fSamplesPerPixel, pDrawParams, pInfoParams);
	dwFirstSample = dwStartSample;
	m_pRightChannel->OnDraw(pDC, rcRightChannel, lOffset, dwFirstSample, dwLength, fSamplesPerPixel, pDrawParams, pInfoParams);

	// Update the refrence so next portion of the wave updates properly
	dwStartSample = dwFirstSample;
	
	return;
}

GUID CStereoWave::GetGUID()
{
	return m_guidStereoWave;
}

void CStereoWave::SetGUID(const GUID& guidSrc)
{
	CWaveNode::CopyGUID(guidSrc, m_guidStereoWave);
	if(m_pLeftChannel)
	{
		m_pLeftChannel->SetGUID(m_guidStereoWave);
	}
	if(m_pRightChannel)
	{
		m_pRightChannel->SetGUID(m_guidStereoWave);
	}
}

// Syncs the WSMPL, WLOOP and Info values for this object from the mono wave values 
// This method must be called after any operation on these structures
void CStereoWave::SyncWaveProperties(bool bCopyFromChannels)
{
	ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
		return;
	if(m_pRightChannel == NULL)
		return;

	if(bCopyFromChannels)
	{
		CopyInfo(&(m_pLeftChannel->m_Info), &m_Info);
		CWave::SetWSMPL(m_pLeftChannel->GetWSMPL());
		CWave::SetWLOOP(m_pLeftChannel->GetWLOOP());
		CWave::SetRSMPL(m_pLeftChannel->GetRSMPL());
		CWave::SetRLOOP(m_pLeftChannel->GetRLOOP());
		m_bPlayLooped = (m_pLeftChannel->IsLooped() && m_pRightChannel->IsLooped());
		m_bCompressed = (m_pLeftChannel->m_bCompressed && m_pRightChannel->m_bCompressed);
		CopyMemory(&m_rWaveformat, &(m_pLeftChannel->m_rWaveformat), sizeof WAVEFORMATEX);
		GetTrueStereoFormat(m_rWaveformat);
		m_dwWaveLength = m_pLeftChannel->m_dwWaveLength;
		m_dwDataSize = GetDataSize();
		m_bStreaming = m_pLeftChannel->IsStreamingWave();
		m_bNoPreroll = m_pLeftChannel->IsNoPrerollWave();
		m_dwReadAheadTime = m_pLeftChannel->GetReadAheadTime();
		m_dwDecompressedStart = m_pLeftChannel->GetDwDecompressedStart(false);

        if(m_bCompressed == false)
        {
            CopyMemory(&m_OriginalWavObj.WaveFormatEx, &m_rWaveformat, sizeof WAVEFORMATEX);
            m_OriginalWavObj.dwDataSize = m_pLeftChannel->GetUncompressedDataSize() * 2;
        }
	}
	else
	{
		CopyInfo(&m_Info, &(m_pLeftChannel->m_Info));
		CopyInfo(&m_Info, &(m_pRightChannel->m_Info));

		WSMPL wsmpl;
		CopyMemory(&wsmpl, &m_rWSMP, sizeof WSMPL);
		
		WLOOP wloop;
		CopyMemory(&wloop, &m_rWLOOP, sizeof WLOOP);

		RSMPL rsmpl;
		CopyMemory(&rsmpl, &m_rRSMPL, sizeof RSMPL);

		RLOOP rloop;
		CopyMemory(&rloop, &m_rRLOOP, sizeof RLOOP);

		m_pLeftChannel->SetWSMPL(wsmpl);
		m_pLeftChannel->SetWLOOP(wloop);
		m_pLeftChannel->SetRSMPL(rsmpl);
		m_pLeftChannel->SetRLOOP(rloop);

		m_pRightChannel->SetWSMPL(wsmpl);
		m_pRightChannel->SetWLOOP(wloop);
		m_pRightChannel->SetRSMPL(rsmpl);
		m_pRightChannel->SetRLOOP(rloop);

		m_pLeftChannel->SetLooped(m_bPlayLooped != 0);
		m_pRightChannel->SetLooped(m_bPlayLooped != 0);

		CopyMemory(&m_pLeftChannel->m_rWaveformat, &m_rWaveformat, sizeof WAVEFORMATEX);
		CopyMemory(&m_pRightChannel->m_rWaveformat, &m_rWaveformat, sizeof WAVEFORMATEX);

		m_pLeftChannel->m_rWaveformat.nChannels = 1;
		m_pLeftChannel->m_rWaveformat.nBlockAlign /= 2;
		
		m_pRightChannel->m_rWaveformat.nChannels = 1;
		m_pRightChannel->m_rWaveformat.nBlockAlign /= 2;

		m_pLeftChannel->m_bCompressed = m_bCompressed;
		m_pRightChannel->m_bCompressed = m_bCompressed;

		m_pLeftChannel->SetStreamingWave(m_bStreaming);
		m_pLeftChannel->SetNoPrerollWave(m_bNoPreroll);
		m_pLeftChannel->SetReadAheadTime(m_dwReadAheadTime);
		m_pLeftChannel->SetDwDecompressedStart(m_dwDecompressedStart);

		m_pRightChannel->SetStreamingWave(m_bStreaming);
		m_pRightChannel->SetNoPrerollWave(m_bNoPreroll);
		m_pRightChannel->SetReadAheadTime(m_dwReadAheadTime);
		m_pRightChannel->SetDwDecompressedStart(m_dwDecompressedStart);

		RefreshWave();

	}
	
}

CString CStereoWave::GetName()
{
	CString sName = "";
	ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
		return sName;
	if(m_pRightChannel == NULL)
		return sName;

	//sName = m_pLeftChannel->GetName();

	return m_Info.m_csName;
}

void CStereoWave::CopyInfo(CInfo* pSrcInfo, CInfo* pDstInfo)
{
	ASSERT(pSrcInfo);
	ASSERT(pDstInfo);
	if(pSrcInfo == NULL)
		return;
	if(pDstInfo == NULL)
		return;

	pDstInfo->m_csName = pSrcInfo->m_csName;
	pDstInfo->m_csComment = pSrcInfo->m_csComment;
	pDstInfo->m_csCopyright = pSrcInfo->m_csCopyright;
	pDstInfo->m_csDate = pSrcInfo->m_csDate;
	pDstInfo->m_csEngineer = pSrcInfo->m_csEngineer;
	pDstInfo->m_csProduct = pSrcInfo->m_csProduct;
	pDstInfo->m_csSoftware = pSrcInfo->m_csSoftware;
	pDstInfo->m_csSubject = pSrcInfo->m_csSubject;
}


DWORD CStereoWave::GetDataSize()
{
	ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
		return 0;
	if(m_pRightChannel == NULL)
		return 0;

	if(!m_bTrueStereo)
	{
		return m_pLeftChannel->GetDataSize() + m_pRightChannel->GetDataSize();
	}

	return m_dwDataSize;
}


DWORD CStereoWave::GetUncompressedDataSize()
{
	ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
		return 0;
	if(m_pRightChannel == NULL)
		return 0;

	if(m_pCollection)
		return m_pLeftChannel->GetUncompressedDataSize() + m_pRightChannel->GetUncompressedDataSize();
	else
		return m_OriginalWavObj.dwDataSize;
}


DWORD CStereoWave::GetCompressedDataSize()
{
	ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
		return 0;
	if(m_pRightChannel == NULL)
		return 0;

	if(m_pCollection)
	{
		return m_pLeftChannel->GetCompressedDataSize() + m_pRightChannel->GetCompressedDataSize();
	}
	else
	{
		return m_CompressedWavObj.dwDataSize;
	}
}

DWORD CStereoWave::GetCompressionFormatTag()
{
	ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
		return 0;
	if(m_pRightChannel == NULL)
		return 0;

	if(m_bCompressed)
	{
		if(m_pCollection)
		{
			DWORD dwLeftChannelCompressionTag = m_pLeftChannel->GetCompressionFormatTag();
			DWORD dwRightChannelCompressionTag = m_pRightChannel->GetCompressionFormatTag();
			
			ASSERT(dwLeftChannelCompressionTag == dwRightChannelCompressionTag);

			if(m_pLeftChannel->GetCompressionFormatTag() == m_pRightChannel->GetCompressionFormatTag())
			{
				return m_pLeftChannel->GetCompressionFormatTag();
			}
		}
		else
		{
			return m_CompressedWavObj.WaveFormatEx.wFormatTag;
		}
	}

	return 0;
}

HRESULT CStereoWave::GetCompressionFormat(WAVEFORMATEX* pwfxCompression)
{
	ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);

	if(pwfxCompression == NULL)
		return E_POINTER;

	if(m_pCollection)
	{
		if(SUCCEEDED(m_pLeftChannel->GetCompressionFormat(pwfxCompression)))
		{
			return S_OK;
		}
	}
	else
	{
		CopyMemory(pwfxCompression, &m_CompressedWavObj.WaveFormatEx, sizeof WAVEFORMATEX);
	}

	return S_OK;
}

void CStereoWave::SetWSMPL(WSMPL& wsmpl)
{
	CWave::SetWSMPL(wsmpl);
	SyncWaveProperties(false);
}

void CStereoWave::SetWLOOP(WLOOP& wloop)
{
	CWave::SetWLOOP(wloop);
	SyncWaveProperties(false);
}


void CStereoWave::SetRSMPL(RSMPL& rsmpl)
{
	CWave::SetRSMPL(rsmpl);
	SyncWaveProperties(false);
}

void CStereoWave::SetRLOOP(RLOOP& rloop)
{
	CWave::SetRLOOP(rloop);
	SyncWaveProperties(false);
}

HRESULT CStereoWave::RemoveSelection(DWORD dwStart, DWORD dwEnd)
{
	ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
	{
		return E_FAIL;
	}

	if(m_pRightChannel == NULL)
	{
		return E_FAIL;
	}

    if(IsTrueStereo() == FALSE)
    {
        if(SUCCEEDED(m_pLeftChannel->RemoveSelection(dwStart, dwEnd)))
        {
            if(SUCCEEDED(m_pRightChannel->RemoveSelection(dwStart, dwEnd)))
            {
                SyncWaveProperties();
                return S_OK;
            }

            return E_FAIL;
        }

        return E_FAIL;
    }

	// Swap the points if necessary
	DWORD dwTemp = dwStart;
	if(dwStart > dwEnd)
	{
		DWORD dwTemp = dwStart;
		dwStart = dwEnd;
		dwEnd = dwTemp;
	}

	int nBytesPerSample = m_rWaveformat.wBitsPerSample / 8;
	int nChannels = m_rWaveformat.nChannels;

	DWORD dwBytesRemoved = 0;
	DWORD dwSamplesRemoved = abs(dwEnd - dwStart) + 1;

	if(FAILED(m_pDataManager->RemoveData(dwStart, dwSamplesRemoved)))
	{
		return E_FAIL;
	}

    m_OriginalWavObj.dwDataSize -= dwSamplesRemoved * nChannels * nBytesPerSample;
	m_dwDataSize = m_OriginalWavObj.dwDataSize;
	m_dwWaveLength -= dwSamplesRemoved;

    RefreshWave();
	UpdateLoopInRegions();

	return S_OK;
}

HRESULT CStereoWave::CopySelectionToClipboard(IStream* pIStream, DWORD dwStart, DWORD dwEnd)
{
	ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
		return E_FAIL;
	if(m_pRightChannel == NULL)
		return E_FAIL;

	ASSERT(pIStream);
	if(pIStream == NULL)
		return E_POINTER;

	// Swap the points if necessary
	if(dwStart > dwEnd)
	{
		DWORD dwTemp = dwStart;
		dwStart = dwEnd;
		dwEnd = dwTemp;
	}

	DWORD dwClipLength = dwEnd - dwStart + 1;

	if(FAILED(WriteDataToClipboard(pIStream, dwStart, dwClipLength)))
	{
		return E_FAIL;
	}

	return S_OK;
}


HRESULT CStereoWave::WriteDataToClipboard(IStream* pIStream, DWORD dwStartSample, DWORD dwClipLength)
{
    if(IsTrueStereo())
    {
        return CWave::WriteDataToClipboard(pIStream, dwStartSample, dwClipLength);
    }

    ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_POINTER;
	}

	IDMUSProdRIFFStream* pIRiffStream = NULL;
	HRESULT hr = AllocRIFFStream( pIStream, &pIRiffStream );
	if(FAILED(hr))
	{
		pIStream->Release();
		return hr;
	}
	
	// Create wave riff header
	MMCKINFO ckMain;
	ckMain.fccType = mmioFOURCC('W','A','V','E');
	hr = pIRiffStream->CreateChunk(&ckMain, MMIO_CREATERIFF);
	if(FAILED(hr))
	{
		return E_FAIL;
	}


	MMCKINFO ck;
	DWORD cb = 0;

	// Create format chunk
	ck.ckid = mmioFOURCC('f','m','t',' ');
	hr = pIRiffStream->CreateChunk(&ck, 0);
	if(FAILED(hr))
	{
		pIRiffStream->Release();
		return E_FAIL;
	}

	hr = pIStream->Write((LPSTR) &(m_rWaveformat), sizeof(WAVEFORMATEX), &cb);	
	if (FAILED(hr) || cb != sizeof(WAVEFORMATEX))
	{
		pIRiffStream->Release();
		return E_FAIL;
	}
	
	if (FAILED(pIRiffStream->Ascend(&ck, 0)))
	{
		pIRiffStream->Release();
		return E_FAIL;
	}
	
	// Save data
	ck.ckid = mmioFOURCC('d','a','t','a') ;
	hr = pIRiffStream->CreateChunk(&ck,0);
	if(FAILED(hr))
	{
		pIRiffStream->Release();
		return E_FAIL;
	}

    BYTE* pbLeft = NULL;
    if(FAILED(m_pLeftChannel->GetChannelData(dwStartSample, dwClipLength, &pbLeft, false)))
	{
		return E_FAIL;
	}

	BYTE* pbRight = NULL;
	if(FAILED(m_pRightChannel->GetChannelData(dwStartSample, dwClipLength, &pbRight, false)))
	{
        delete[] pbLeft;
		return E_FAIL;
	}

	// Interleave the data
	BYTE* pbStereo = NULL;
	DWORD dwStereoSize = dwClipLength * 2 * (m_rWaveformat.wBitsPerSample / 8);
	if(FAILED(SafeAllocBuffer(&pbStereo, dwStereoSize)))
	{
        delete[] pbLeft;
        delete[] pbRight;
		return E_OUTOFMEMORY;
	}

	if(FAILED(InterleaveBuffersForStereoData(pbLeft, pbRight, pbStereo, m_rWaveformat, dwStereoSize)))
	{
        delete[] pbLeft;
        delete[] pbRight;
        delete[] pbStereo;
		return E_FAIL;
	}


	if(FAILED(pIStream->Write((LPSTR) pbStereo, dwStereoSize, &cb)))	
	{
        delete[] pbLeft;
        delete[] pbRight;
		delete[] pbStereo;
		return E_FAIL;
	}

	delete[] pbStereo;
    delete[] pbLeft;
    delete[] pbRight;

	hr = pIRiffStream->Ascend(&ck, 0);
	if(FAILED(hr))
	{
		pIRiffStream->Release();
		return E_FAIL;
	}

	pIRiffStream->Ascend(&ckMain, 0);
	pIRiffStream->Release();

	return S_OK;

}


HRESULT CStereoWave::PasteSelectionFromClipboard(IStream* pIStream, DWORD dwStart)
{
	ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
		return E_FAIL;
	if(m_pRightChannel == NULL)
		return E_FAIL;
	ASSERT(pIStream);
	if(pIStream == NULL)
		return E_POINTER;

	BYTE* pbData = NULL;
	DWORD dwDataSize = 0;
	WAVEFORMATEX wfxClip;

	if(FAILED(CWave::ReadDataFromClipbaord(pIStream, wfxClip, &pbData, dwDataSize)) || pbData == NULL || dwDataSize == 0)
	{
		return E_FAIL;
	}

	UINT nClipSampleSize = wfxClip.wBitsPerSample / 8;
	UINT nClipChannels = wfxClip.nChannels;

	DWORD dwClipLength = (dwDataSize / nClipSampleSize) / nClipChannels;

	// Insert method deletes the passed buffer
	return InsertWaveData(pbData, wfxClip, dwClipLength, dwStart);
}


HRESULT CStereoWave::InsertWaveData(BYTE* pbData, WAVEFORMATEX wfxDataFormat, DWORD dwSamples, DWORD dwStart)
{
	ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
		return E_FAIL;
	if(m_pRightChannel == NULL)
		return E_FAIL;

	UINT nClipChannels = wfxDataFormat.nChannels;
	UINT nClipSampleSize = wfxDataFormat.wBitsPerSample / 8;

	DWORD dwNewBufferSize = 0;
	DWORD dwNumNewBytes = 0;

    if(IsTrueStereo() == FALSE)
    {
        DWORD dwDataSize = dwSamples * nClipSampleSize * nClipChannels;
        BYTE* pbChannel = NULL;
        if(FAILED(CWave::SafeAllocBuffer(&pbChannel, dwDataSize)))
        {
            delete[] pbData;
            return E_OUTOFMEMORY;
        }

        CopyMemory(pbChannel, pbData, dwDataSize);
        if(SUCCEEDED(m_pLeftChannel->InsertWaveData(pbChannel, wfxDataFormat, dwSamples, dwStart)))
        {
            if(FAILED(CWave::SafeAllocBuffer(&pbChannel, dwDataSize)))
            {
                delete[] pbData;
                return E_OUTOFMEMORY;
            }
            CopyMemory(pbChannel, pbData, dwDataSize);

            if(SUCCEEDED(m_pRightChannel->InsertWaveData(pbChannel, wfxDataFormat, dwSamples, dwStart)))
            {
                SyncWaveProperties();
                
                delete[] pbData;
                return S_OK;
            }

            delete[] pbData;
            return E_FAIL;
        }

        delete[] pbData;
        return E_FAIL;
    }


	if(nClipChannels == 1)
	{
		// Make an interleaved stereo buffer out of the mono buffer 
		BYTE* pbStereoData = NULL;
		DWORD dwDataSize = dwSamples * nClipSampleSize;
		DWORD dwStereoSize = dwDataSize * 2;
		if(FAILED(SafeAllocBuffer(&pbStereoData, dwStereoSize)))
		{
			return E_OUTOFMEMORY;
		}
		
		BYTE* pbCopy = NULL;
		if(FAILED(SafeAllocBuffer(&pbCopy, dwDataSize)))
		{
            delete[] pbData;
			return E_OUTOFMEMORY;
		}
		CopyMemory(pbCopy, pbData, dwDataSize);

		if(FAILED(InterleaveBuffersForStereoData(pbData, pbCopy, pbStereoData, wfxDataFormat, dwStereoSize)))
		{
            delete[] pbData;
			delete[] pbCopy;
			delete[] pbStereoData;
			return E_FAIL;
		}

		delete[] pbData;
		delete[] pbCopy;
		pbData = pbStereoData;
		nClipChannels = 2;
	}

	DWORD dwClipSize = dwSamples * nClipSampleSize * nClipChannels;

	if(m_rWaveformat.wBitsPerSample == 8)
	{
		dwNumNewBytes = dwSamples * 2;
	}
	else
	{
		dwNumNewBytes = (dwSamples << 1) * 2;
	}

	// Massage the data if the sample sizes are different
	if(m_rWaveformat.wBitsPerSample != wfxDataFormat.wBitsPerSample)
	{
		DWORD dwIndex = 0;
		if(m_rWaveformat.wBitsPerSample == 8) 
		{
			BYTE* pbNewBuffer = NULL;
			if(FAILED(SafeAllocBuffer(&pbNewBuffer, dwNumNewBytes)))
			{
				delete[] pbData;
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
				delete[] pbData;
				return E_OUTOFMEMORY;
			}

			for(dwIndex = 0; dwIndex < dwClipSize; dwIndex++)
			{
				pnNewBuffer[dwIndex] = *(pbData + dwIndex) < 128 ? 
					- MulDiv(abs(*(pbData + dwIndex) - 128), 32768, 128) :
					+ MulDiv(*(pbData + dwIndex) - 128, 32767, 127);
			}
			
			delete[] pbData;
			pbData = (BYTE*)pnNewBuffer;
		}
	}

	if(FAILED(m_pDataManager->InsertData(dwStart, pbData, dwSamples)))
	{
		return E_FAIL;
	}

	delete[] pbData;

	m_dwWaveLength += dwSamples;
	m_OriginalWavObj.dwDataSize += dwNumNewBytes;
    m_dwDataSize = m_OriginalWavObj.dwDataSize;

	RefreshWave();
	
	return S_OK;
}

	
HRESULT CStereoWave::Fade(bool bFadeDirection, DWORD dwStart, DWORD dwEnd)
{
	ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
		return E_FAIL;
	if(m_pRightChannel == NULL)
		return E_FAIL;

    if(IsTrueStereo() == FALSE)
    {
        if(FAILED(m_pLeftChannel->Fade(bFadeDirection, dwStart, dwEnd)))
        {
            return E_FAIL;
        }
        
        if(FAILED(m_pRightChannel->Fade(bFadeDirection, dwStart, dwEnd)))
        {
            return E_FAIL;
        }
    }
    else
    {
	    if(FAILED(CWave::Fade(bFadeDirection, dwStart, dwEnd)))
	    {
		    return E_FAIL;
	    }
    }

	if(FAILED(RefreshWave()))
	{
		return E_FAIL;
	}

	return S_OK;
}

	
HRESULT CStereoWave::InsertSilence(DWORD dwStart, DWORD dwLength)
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

	dwBytesPerSample *= 2;

	ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
		return E_FAIL;
	if(m_pRightChannel == NULL)
		return E_FAIL;

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

	
HRESULT CStereoWave::Resample(DWORD dwNewSampleRate)
{
	ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
		return E_FAIL;
	if(m_pRightChannel == NULL)
		return E_FAIL;

	if( m_rWaveformat.nSamplesPerSec == dwNewSampleRate )
	{
		return S_OK;
	}

    if(IsTrueStereo() == FALSE)
    {
        if(FAILED(m_pLeftChannel->Resample(dwNewSampleRate)))
        {
            return E_FAIL;
        }
        
        if(FAILED(m_pRightChannel->Resample(dwNewSampleRate)))
        {
            return E_FAIL;
        }

		// Now update the stereo wave with the new sample rate
		SyncWaveProperties(true);
    }
    else
    {
	    if(FAILED(CWave::Resample(dwNewSampleRate)))
	    {
		    return E_FAIL;
	    }

		// Update the individual waves with the new sample rate
		SyncWaveProperties(false);
    }

	if(FAILED(RefreshWave()))
	{
		return E_FAIL;
	}

	return S_OK;
}


HRESULT CStereoWave::CopyLoop()
{
	ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
		return E_FAIL;
	if(m_pRightChannel == NULL)
		return E_FAIL;

	// Wave's not looped?
	if(m_bPlayLooped == false)
	{
		return E_FAIL;
	}

	DWORD dwLoopLength = m_rWLOOP.ulLength;
	DWORD dwStartSample = m_rWLOOP.ulStart;
	DWORD dwEndSample = dwStartSample + dwLoopLength;

	BYTE* pbBuffer = NULL;
	DWORD cb = 0;
	if(FAILED(GetData(dwStartSample, dwLoopLength, &pbBuffer, cb)))
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

	SyncWaveProperties(false);

	UpdateLoopInRegions();

	// Dirty the collection so we know the file gets saved
	return S_OK;
}


HRESULT CStereoWave::SetLoop(DWORD dwStart, DWORD dwEnd)
{
	ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
		return E_FAIL;
	if(m_pRightChannel == NULL)
		return E_FAIL;

	if(SUCCEEDED(m_pLeftChannel->SetLoop(dwStart, dwEnd)))
	{
		if(SUCCEEDED(m_pRightChannel->SetLoop(dwStart, dwEnd)))
		{
			// RememberLoopAfterCompression was called as part of the SetLoop method,
			// so we just need to fix it in our wave structure
			CWave::RememberLoopAfterCompression();
			SyncWaveProperties(true);
			UpdateDummyInstrument();
			return S_OK;
		}
	}

	return E_FAIL;
}

void CStereoWave::RememberLoopBeforeCompression()
{
    ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
		return;
	if(m_pRightChannel == NULL)
		return;

    CWave::RememberLoopBeforeCompression();
    m_pLeftChannel->RememberLoopBeforeCompression();
    m_pRightChannel->RememberLoopBeforeCompression();    
}

void CStereoWave::RevertToLoopBeforeCompression()
{
    ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
		return;
	if(m_pRightChannel == NULL)
		return;

    CWave::RevertToLoopBeforeCompression();
    m_pLeftChannel->RevertToLoopBeforeCompression();
    m_pRightChannel->RevertToLoopBeforeCompression();
}

void CStereoWave::RememberLoopAfterCompression()
{
    ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
		return;
	if(m_pRightChannel == NULL)
		return;

    CWave::RememberLoopAfterCompression();
    m_pLeftChannel->RememberLoopAfterCompression();
    m_pRightChannel->RememberLoopAfterCompression();    
}

/* Reverts to the saved decompressed start and loop points */
void CStereoWave::RevertToAfterCompression()
{
    ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
		return;
	if(m_pRightChannel == NULL)
		return;

    CWave::RevertToAfterCompression();
    m_pLeftChannel->RevertToAfterCompression();
    m_pRightChannel->RevertToAfterCompression();
}

HRESULT CStereoWave::InterleaveBuffersForStereoData(BYTE* pbLeft, BYTE* pbRight, BYTE* pbStereoData, WAVEFORMATEX wfxData, DWORD dwDataSize)
{
	ASSERT(pbLeft);
	ASSERT(pbRight);
	ASSERT(pbStereoData);
	if(pbLeft == NULL || pbRight == NULL || pbStereoData == NULL)
	{
		return E_POINTER;
	}

	UINT nBytesPerSample = wfxData.wBitsPerSample / 8;
	for(DWORD dwByteCount = 0; dwByteCount < dwDataSize; dwByteCount += nBytesPerSample * 2)
	{
		CopyMemory(pbStereoData + dwByteCount, pbLeft, nBytesPerSample);
		CopyMemory(pbStereoData + dwByteCount + nBytesPerSample, pbRight, nBytesPerSample);

		pbLeft += nBytesPerSample;
		pbRight += nBytesPerSample;
	}

	return S_OK;
}


HRESULT CStereoWave::InterleaveChannelsForStereoData()
{
	ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
		return E_FAIL;
	if(m_pRightChannel == NULL)
		return E_FAIL;

	// Clean the original data if any
	if(m_OriginalWavObj.pbData)
	{
		delete[] m_OriginalWavObj.pbData;
		m_OriginalWavObj.pbData = NULL;
		m_OriginalWavObj.dwDataSize = 0;
	}


	BYTE* pbLeftChannelData = (BYTE*)m_pLeftChannel->GetWaveData();
	BYTE* pbRightChannelData = (BYTE*)m_pRightChannel->GetWaveData();

	// This might be a newly created empty wave
	if(pbLeftChannelData == NULL || pbRightChannelData == NULL)
	{
		m_OriginalWavObj.pbData = NULL;
		m_OriginalWavObj.dwDataSize = 0;

		return S_OK;
	}

	DWORD dwDataSize = GetDataSize();
	BYTE* pbData = new BYTE[dwDataSize];
	if(pbData == NULL)
	{
		return E_OUTOFMEMORY;
	}
	ZeroMemory(pbData, dwDataSize);

	int nBytesPerSample = m_rWaveformat.wBitsPerSample / 8;

	for(DWORD dwByteCount = 0; dwByteCount < dwDataSize; dwByteCount += nBytesPerSample * 2)
	{
		CopyMemory(pbData + dwByteCount, pbLeftChannelData, nBytesPerSample);
		CopyMemory(pbData + dwByteCount + nBytesPerSample, pbRightChannelData, nBytesPerSample);

		pbLeftChannelData += nBytesPerSample;
		pbRightChannelData += nBytesPerSample;
	}

	m_OriginalWavObj.pbData = pbData;
	m_OriginalWavObj.dwDataSize = dwDataSize;

	return S_OK;
}



HRESULT CStereoWave::ConvertStereoData(const DMUSP_WAVEOBJECT& sourceWave, const WAVEFORMATEX* pwfxDst, DMUSP_WAVEOBJECT& convertedWave, bool bCompress)
{
	ASSERT(m_pLeftChannel);
	if(m_pLeftChannel == NULL)
		return E_FAIL;

	ASSERT(m_pRightChannel);
	if(m_pRightChannel == NULL)
		return E_FAIL;

	// This must be a stereo format
	if(pwfxDst->nChannels != 2)
		return E_FAIL;

	if(FAILED(CWave::ConvertWave(sourceWave, pwfxDst, convertedWave, bCompress)))
	{
		return E_FAIL;
	}

	return S_OK;
}


void CStereoWave::GetTrueStereoFormat(WAVEFORMATEX& wfxPCM)
{
	CopyMemory(&wfxPCM, &m_rWaveformat, sizeof WAVEFORMATEX);
	
	if(wfxPCM.nChannels < 2)
	{
		wfxPCM.nChannels  = 2;
	}

	if((wfxPCM.nBlockAlign == 1 && wfxPCM.wBitsPerSample == 8) ||
		(wfxPCM.nBlockAlign == 2 && wfxPCM.wBitsPerSample == 16))
	{
		wfxPCM.nBlockAlign *= 2;
	}

    wfxPCM.nAvgBytesPerSec = wfxPCM.nSamplesPerSec * wfxPCM.nBlockAlign;

	wfxPCM.cbSize = 0;

	return;
}


HRESULT CStereoWave::UpdateChannelsForStereoCompression()
{
	ASSERT(m_pLeftChannel);
	if(m_pLeftChannel == NULL)
		return E_FAIL;

	ASSERT(m_pRightChannel);
	if(m_pRightChannel == NULL)
		return E_FAIL;

	// Don't do anything if the wave is not compressed
	if(!m_bCompressed)
		return E_FAIL;

	
	if(SUCCEEDED(m_pLeftChannel->UpdateDataForStereoCompression(m_DecompressedWavObj)))
	{
		if(SUCCEEDED(m_pRightChannel->UpdateDataForStereoCompression(m_DecompressedWavObj)))
		{
			return S_OK;
		}
	}
	
	return E_FAIL;
}


HRESULT CStereoWave::ReadFormatChunk(IDMUSProdRIFFStream* pIRiffStream)
{
	if(pIRiffStream == NULL)
		return E_POINTER;

	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream == NULL)
		return E_FAIL;

	DWORD cb = 0;
	WAVEFORMATEX wfxTemp;
	HRESULT hr = pIStream->Read( &wfxTemp, sizeof WAVEFORMATEX, &cb );
	if(FAILED(hr) || cb != sizeof WAVEFORMATEX)
	{
		pIStream->Release();
		return E_FAIL;
	}
	
	// This must be a compressed wav huh?
	if(m_bCompressed && m_bLoadingRuntime)
	{
		BYTE* pbExtraInfo = NULL;
		
		// So we need the extra info bits
		if(wfxTemp.cbSize > 0)
		{
			pbExtraInfo = new BYTE[wfxTemp.cbSize];
			if(pbExtraInfo == NULL)
			{
				pIStream->Release();
				return E_OUTOFMEMORY;
			}
			ZeroMemory(pbExtraInfo, wfxTemp.cbSize);

			hr = pIStream->Read(pbExtraInfo, wfxTemp.cbSize, &cb);
			if(FAILED(hr) || cb != wfxTemp.cbSize)
			{
				delete[] pbExtraInfo;
				pIStream->Release();
				return(hr);
			}
		}

		if(m_CompressedWavObj.pbExtractWaveFormatData)
		{
			delete[] m_CompressedWavObj.pbExtractWaveFormatData;
			m_CompressedWavObj.pbExtractWaveFormatData = NULL;
		}

		CopyMemory(&(m_CompressedWavObj.WaveFormatEx), &wfxTemp, sizeof wfxTemp);
		m_CompressedWavObj.pbExtractWaveFormatData = pbExtraInfo;
	}
	else if(wfxTemp.wFormatTag == WAVE_FORMAT_PCM)
	{
		CopyMemory(&m_rWaveformat, &wfxTemp, sizeof wfxTemp);
		m_rWaveformat.cbSize = 0;
		CopyMemory(&m_OriginalWavObj.WaveFormatEx, &m_rWaveformat, sizeof WAVEFORMATEX);
	}
	else 
	{
		AfxMessageBox(IDS_WAVE_NOT_PCM, MB_ICONEXCLAMATION); 
		pIStream->Release();
		return E_FAIL;
	}

	pIStream->Release();
	return S_OK;
}

HRESULT CStereoWave::ReadWSMPLChunk(IDMUSProdRIFFStream* pIRiffStream, DWORD dwChunkSize)
{
	if(pIRiffStream == NULL)
		return E_POINTER;

	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream == NULL)
		return E_FAIL;

	DWORD cSize = 0;
	DWORD cb = 0;

	if(dwChunkSize < (sizeof WSMPL + sizeof WLOOP) )
	{
		cSize = dwChunkSize;
	}
	else
	{
		cSize = sizeof WSMPL + sizeof WLOOP;
	}
	
	HRESULT hr = pIStream->Read( &m_rWSMP, cSize, &cb );
	if( FAILED(hr) || cb != cSize )
	{
		pIStream->Release();
		return E_FAIL;
	}
			
	pIStream->Release();
	return S_OK;
}


HRESULT CStereoWave::ReadRSMPLChunk(IDMUSProdRIFFStream* pIRiffStream, DWORD dwChunkSize)
{
	if(pIRiffStream == NULL)
		return E_POINTER;

	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream == NULL)
		return E_FAIL;

	DWORD cSize = 0;
	DWORD cb = 0;
	
	if(dwChunkSize < (sizeof RSMPL + sizeof RLOOP))
	{
		cSize = dwChunkSize;
	}
	else
	{
		cSize = (sizeof RSMPL + sizeof RLOOP);
	}

    HRESULT hr = pIStream->Read( &m_rRSMPL, cSize, &cb );
	if(FAILED(hr) || cb != cSize )
	{
		pIStream->Release();
		return E_FAIL;
	}

	pIStream->Release();
	return S_OK;
}

HRESULT	CStereoWave::GetPlayTimeForTempo(int nTempo, DWORD& dwPlayTime)
{
	ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
		return E_FAIL;
	if(m_pRightChannel == NULL)
		return E_FAIL;
	
	DWORD dwLeftChannelPlayTime = m_pLeftChannel->GetPlayTimeForTempo(nTempo, dwPlayTime);
	DWORD dwRightChannelPlayTime = m_pRightChannel->GetPlayTimeForTempo(nTempo, dwPlayTime);

	// Resturn the longest play time
	dwLeftChannelPlayTime > dwRightChannelPlayTime ? dwPlayTime = dwLeftChannelPlayTime : dwPlayTime = dwRightChannelPlayTime;

	return S_OK;
}

void CStereoWave::SnapToZero(DWORD& dwSample, UINT nDirection)
{
	ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
		return;
	if(m_pRightChannel == NULL)
		return;

	m_pLeftChannel->SnapToZero(dwSample, nDirection);
}

HRESULT CStereoWave::RefreshWave()
{
	ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
		return E_FAIL;
	if(m_pRightChannel == NULL)
		return E_FAIL;

	// Set the data for the channels
	DMUSP_WAVEOBJECT stereoWave;
	stereoWave.dwDataSize = m_OriginalWavObj.dwDataSize;
	stereoWave.WaveFormatEx = m_OriginalWavObj.WaveFormatEx;
	stereoWave.pbExtractWaveFormatData = NULL;
	
	if(FAILED(m_pRightChannel->SetUncompressedData(stereoWave)))
	{
		return E_FAIL;
	}

	if(FAILED(m_pLeftChannel->SetUncompressedData(stereoWave)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CString CStereoWave::GetUndoMenuText(bool bRedo)
{
    ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
    {
		return "";
    }

	if(m_pRightChannel == NULL)
    {
		return "";
    }

    ASSERT(m_pDataManager);
    if(m_pDataManager == NULL)
    {
        return "";
    }

    if(IsTrueStereo() == FALSE)
    {
        m_pLeftChannel->GetUndoMenuText(bRedo);
        return m_pRightChannel->GetUndoMenuText(bRedo);
    }

    return CWave::GetUndoMenuText(bRedo);
}

HRESULT CStereoWave::SaveUndoState(UINT uUndoStrID)
{
	ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
    {
		return E_FAIL;
    }

	if(m_pRightChannel == NULL)
    {
		return E_FAIL;
    }

    ASSERT(m_pDataManager);
    if(m_pDataManager == NULL)
    {
        return E_FAIL;
    }

    if(IsTrueStereo() == FALSE)
    {
        if(SUCCEEDED(m_pLeftChannel->SaveUndoState(uUndoStrID)))
        {
            return m_pRightChannel->SaveUndoState(uUndoStrID);
        }

        return E_FAIL;
    }

    return CWave::SaveUndoState(uUndoStrID);
}

HRESULT CStereoWave::Undo()
{
    ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
    {
		return E_FAIL;
    }

	if(m_pRightChannel == NULL)
    {
		return E_FAIL;
    }

    ASSERT(m_pDataManager);
    if(m_pDataManager == NULL)
    {
        return E_FAIL;
    }

    if(IsTrueStereo() == FALSE)
    {
        if(SUCCEEDED(m_pLeftChannel->Undo()))
        {
            if(SUCCEEDED(m_pRightChannel->Undo()))
            {
                SyncWaveProperties();
                return S_OK;
            }

            return E_FAIL;
        }

        return E_FAIL;
    }

    return CWave::Undo();
}

HRESULT CStereoWave::Redo()
{
    ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
    {
		return E_FAIL;
    }

	if(m_pRightChannel == NULL)
    {
		return E_FAIL;
    }

    ASSERT(m_pDataManager);
    if(m_pDataManager == NULL)
    {
        return E_FAIL;
    }

    if(IsTrueStereo() == FALSE)
    {
        if(SUCCEEDED(m_pLeftChannel->Redo()))
        {
            if(SUCCEEDED(m_pRightChannel->Redo()))
            {
                SyncWaveProperties();
                return S_OK;
            }

            return E_FAIL;
        }

        return E_FAIL;
    }

    return CWave::Redo();
}


HRESULT CStereoWave::PopUndoState()
{
    ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
    {
		return E_FAIL;
    }

	if(m_pRightChannel == NULL)
    {
		return E_FAIL;
    }

    ASSERT(m_pDataManager);
    if(m_pDataManager == NULL)
    {
        return E_FAIL;
    }

    if(IsTrueStereo() == FALSE)
    {
        if(SUCCEEDED(m_pLeftChannel->PopUndoState()))
        {
            if(SUCCEEDED(m_pRightChannel->PopUndoState()))
            {
                SyncWaveProperties();
                return S_OK;
            }

            return E_FAIL;
        }

        return E_FAIL;
    }

    return CWave::PopUndoState();


}

void CStereoWave::UpdateOnUndo(HEADER_INFO headerInfo)
{
    ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
    {
		return;
    }

	if(m_pRightChannel == NULL)
    {
		return;
    }

	// Update the base structures
    if(IsTrueStereo())
    {
        m_pLeftChannel->m_bCompressed = headerInfo.m_bCompressed;
        m_pRightChannel->m_bCompressed = headerInfo.m_bCompressed;

	    CWave::UpdateOnUndo(headerInfo);
	    
	    // Now update the individual channels
	    SyncWaveProperties(false);

	    if(SUCCEEDED(UpdateHeaderStream()))
	    {
		    m_pWaveNode->ReloadDirectSoundWave();
	    }
    }
    else
    {
        m_pLeftChannel->UpdateOnUndo(headerInfo);
        m_pRightChannel->UpdateOnUndo(headerInfo);

        // Now update the stereo wave
	    SyncWaveProperties(true);
    }
}

HRESULT	CStereoWave::WriteRIFFHeaderToStream(IStream* pIStream, DWORD* pdwBytesWritten)
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
	
	// RIFF chunk size = 'WAVE' + 'wavhXXXX' + _DMUS_IO_WAVE_HEADER + 'fmt XXXX' + PCM-WAVEFORMATEX + 'dataXXXX' + actual data size
	ckMain.cksize = 4 + 8 + sizeof(_DMUS_IO_WAVE_HEADER) + 8 + sizeof(WAVEFORMATEX) + 8 + m_dwDataSize;
	if(FAILED(pIRiffStream->CreateChunk(&ckMain, MMIO_CREATERIFF)))
	{
		pIRiffStream->Release();
		return E_FAIL;
	}

	// Save the 'wavh' chunk that keeps the streaming info
	MMCKINFO ck;
	ck.ckid = DMUS_FOURCC_WAVEHEADER_CHUNK;
	HRESULT hr = pIRiffStream->CreateChunk(&ck, 0);
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

	if(FAILED(WriteStereoFormat(pIRiffStream, false)))
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
		pIStream->Release();
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

BOOL CStereoWave::IsTrueStereo()
{
	ASSERT(m_pWaveNode);
	if(m_pWaveNode == NULL)
	{
		return FALSE;
	}

	if(m_pWaveNode->IsInACollection() == FALSE)
	{
		return TRUE;
	}

	return m_bTrueStereo;
}


HRESULT CStereoWave::CloseSourceHandler()
{
	ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
	{
		return E_FAIL;
	}
	if(m_pRightChannel == NULL)
	{
		return E_FAIL;
	}
	if(m_pDataManager == NULL)
	{
		return E_FAIL;
	}

	EnterCriticalSection(&m_pWaveNode->m_CriticalSection);
	if(FAILED(m_pDataManager->CloseSourceHandler()))
	{
		LeaveCriticalSection(&m_pWaveNode->m_CriticalSection);
		return E_FAIL;
	}
	LeaveCriticalSection(&m_pWaveNode->m_CriticalSection);

	if(SUCCEEDED(m_pLeftChannel->CloseSourceHandler()))
	{
		if(SUCCEEDED(m_pRightChannel->CloseSourceHandler()))
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CStereoWave::OnSourceRenamed(CString sNewName)
{
	ASSERT(m_pDataManager);
	if(m_pDataManager == NULL)
	{
		return E_FAIL;
	}

	EnterCriticalSection(&m_pWaveNode->m_CriticalSection);
	if(FAILED(m_pDataManager->OnSourceRenamed(sNewName)))
	{
		LeaveCriticalSection(&m_pWaveNode->m_CriticalSection);
		return E_FAIL;
	}
	LeaveCriticalSection(&m_pWaveNode->m_CriticalSection);

	if(SUCCEEDED(m_pLeftChannel->OnSourceRenamed(sNewName)))
	{
		if(SUCCEEDED(m_pRightChannel->OnSourceRenamed(sNewName)))
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CStereoWave::FileNameChanged(CString sNewFileName)
{
	ASSERT(m_pLeftChannel);
	ASSERT(m_pRightChannel);
	if(m_pLeftChannel == NULL)
	{
		return E_FAIL;
	}
	if(m_pRightChannel == NULL)
	{
		return E_FAIL;
	}

	ASSERT(m_pDataManager);
	if(m_pDataManager == NULL)
	{
		return E_FAIL;
	}

	if(FAILED(m_pDataManager->Initialize(sNewFileName, m_dwHeaderOffset, m_dwDataOffset, false)))
	{
		return E_FAIL;
	}

	if(FAILED(m_pLeftChannel->FileNameChanged(sNewFileName)))
	{
		return E_FAIL;
	}

	if(FAILED(m_pRightChannel->FileNameChanged(sNewFileName)))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CStereoWave::PrepareForSave(CString sNewFileName)	
{
	ASSERT(m_pDataManager);
	if(m_pDataManager == NULL)
	{
		return E_POINTER;
	}
	
	// We need to rename ourselves if we're NOT in a collection
	if(m_pCollection == NULL)
	{
		if(FAILED(m_pLeftChannel->CloseSourceHandler()))
		{
			return E_FAIL;
		}

		if(FAILED(m_pRightChannel->CloseSourceHandler()))
		{
			return E_FAIL;
		}

		if(FAILED(m_pDataManager->RenameSource(sNewFileName)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CStereoWave::CleanupAfterSave(CString sFileName)
{
	ASSERT(m_pDataManager);
	if(m_pDataManager == NULL)
	{
		return E_FAIL;
	}

	ASSERT(m_pLeftChannel);
	if(m_pLeftChannel == NULL)
	{
		return E_UNEXPECTED;
	}

	ASSERT(m_pRightChannel);
	if(m_pRightChannel == NULL)
	{
		return E_UNEXPECTED;
	}

	if(FAILED(m_pDataManager->Initialize(sFileName, m_dwHeaderOffset, m_dwDataOffset, true)))
	{
		return E_FAIL;
	}

	if(FAILED(m_pLeftChannel->CleanupAfterSave(sFileName)))
	{
		return E_FAIL;
	}

	if(FAILED(m_pRightChannel->CleanupAfterSave(sFileName)))
	{
		return E_FAIL;
	}

	// Set the name for the wave node 
	m_pWaveNode->SetFileName(sFileName);

    m_pWaveNode->SetSourceInTempFile(FALSE);

    if(m_pCollection)
    {
        m_bTrueStereo = FALSE;
    }

	return S_OK;
}

HRESULT CStereoWave::FindBestLoop(DWORD dwLoopStart, DWORD dwFindStart, DWORD dwFindEnd, int nFindWindow, DWORD* pdwBestLoopEnd)
{
	ASSERT(m_pLeftChannel);
	if(m_pLeftChannel == NULL)
	{
		return E_UNEXPECTED;
	}

	ASSERT(m_pRightChannel);
	if(m_pRightChannel == NULL)
	{
		return E_UNEXPECTED;
	}

	if(FAILED(m_pLeftChannel->FindBestLoop(dwLoopStart, dwFindStart, dwFindEnd, nFindWindow, pdwBestLoopEnd)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CStereoWave::SetCollection(CCollection* pCollection)
{
	ASSERT(m_pLeftChannel);
	if(m_pLeftChannel == NULL)
	{
		return;
	}

	ASSERT(m_pRightChannel);
	if(m_pRightChannel == NULL)
	{
		return;
	}


	CWave::SetCollection(pCollection);

	m_pLeftChannel->SetCollection(pCollection);
	m_pRightChannel->SetCollection(pCollection);

}

void CStereoWave::SetPreviousRegionLinkIDs(DWORD dwLeft, DWORD dwRight)
{
	ASSERT(m_pLeftChannel);
	if(m_pLeftChannel == NULL)
	{
		return;
	}

	ASSERT(m_pRightChannel);
	if(m_pRightChannel == NULL)
	{
		return;
	}

	m_pLeftChannel->SetPreviousRegionLinkID(dwLeft);
	m_pRightChannel->SetPreviousRegionLinkID(dwRight);
}

/* returns given channel */
CMonoWave *CStereoWave::GetChannel(LONG iChannel)
{
	switch (iChannel)
		{
		case 0:
			return GetLeftChannel();

		case 1:
			return GetRightChannel();

		default:
			ASSERT(FALSE); // unexpected channel number
			return NULL;
		}
}

/* sets wavelink parameters according to channel number */
void CStereoWave::SetWaveLinkPerChannel(LONG iChannel, WAVELINK *pwl)
{
	ASSERT(pwl);

	// set multichannel
	if ((pwl->fusOptions & F_WAVELINK_MULTICHANNEL) != 0)
		pwl->ulChannel = 1 << iChannel;

	// only one channel can be master
	if (iChannel != 0)
		pwl->fusOptions &= ~F_WAVELINK_PHASE_MASTER;

	// set wave ID
	CMonoWave *pMonoWave = GetChannel(iChannel);
	pwl->ulTableIndex = pMonoWave->GetWaveID();
}

