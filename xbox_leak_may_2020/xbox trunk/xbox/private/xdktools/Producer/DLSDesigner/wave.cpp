//////////////////////////////////////////////////////////////////////
//
// Wave.cpp : implementation file
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <mmsystem.h>
#include <mmreg.h>
#include <msacm.h>
#include "DLSDesignerDLL.h"
#include "WaveNode.h"
#include "Wave.h"
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
#include "resource.h"
#include "Articulation.h"

#include "WaveDataManager.h"

#include "DlsDefsPlus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define RESAMPLE_SIZE_CHANGE( dwNewSampleRate, dwOldSampleRate, dwOldSize ) ((__int64(dwNewSampleRate) * __int64(dwOldSize) + __int64(dwOldSampleRate)/2)/__int64(dwOldSampleRate))


CWaveCompressionManager	CWave::m_CompressionManager;

//////////////////////////////////////////////////////////////////////
//
//	CWave::CWave();
//
//////////////////////////////////////////////////////////////////////
CWave::CWave(CWaveNode* pNode, DWORD dwSampleRate, UINT nSampleSize, UINT nChannels) :
m_bPlayLooped(false),
m_bCompressed(false),
m_bLoadingRuntime(false),
m_dwPreviousRegionLinkID(0),
m_pWavePropPgMgr(NULL),
m_bIsPlaying(FALSE),
m_pDataManager(NULL),
m_pDummyInstrument(NULL), 
m_pIMemHeaderStream(NULL),
m_bStreaming(FALSE),
m_bNoPreroll(FALSE),
m_bUseFormatSuggest(TRUE),
m_dwReadAheadTime(DEFAULT_READAHEAD_TIME),
m_dwId(0),
m_dwDecompressedStart(0)
{
	ASSERT( pNode != NULL );

    m_lRef = 0;
	AddRef();

	m_pWaveNode = pNode;
    m_pCollection = NULL;

	m_nTimePlayed = 0;

	m_dwWaveLength = 0;
	m_dwDataSize = 0;

	m_rWaveformat.wFormatTag = WAVE_FORMAT_PCM;
    m_rWaveformat.nChannels = WORD(nChannels);
    m_rWaveformat.nSamplesPerSec = dwSampleRate;
	m_rWaveformat.wBitsPerSample = WORD(nSampleSize);
    m_rWaveformat.nAvgBytesPerSec = dwSampleRate * (nSampleSize / 8);
    m_rWaveformat.nBlockAlign = nChannels * (nSampleSize / 8);
	m_rWaveformat.cbSize = 0;
    
	m_rRSMPL.dwManufacturer = 0x03000041;   // Microsoft.
    m_rRSMPL.dwProduct = 0;
    m_rRSMPL.dwSamplePeriod = 45351;        // 22 kHz.
    m_rRSMPL.dwMIDIUnityNote = 60;
    m_rRSMPL.dwMIDIPitchFraction = 0;
    m_rRSMPL.dwSMPTEFormat = 0;
    m_rRSMPL.dwSMPTEOffset = 0;
    m_rRSMPL.cSampleLoops = 0;
    m_rRSMPL.cbSamplerData = 0;
    
	m_rRLOOP.dwIdentifier = 0;
    m_rRLOOP.dwType = 0;
    m_rRLOOP.dwStart = 0;
    m_rRLOOP.dwEnd = 0;
    m_rRLOOP.dwFraction = 0;
    m_rRLOOP.dwPlayCount = 0;
    
	m_rWSMP.cbSize = sizeof(WSMPL);
    m_rWSMP.usUnityNote = 60;
    m_rWSMP.sFineTune = 0;
    m_rWSMP.lAttenuation = 0;
    m_rWSMP.fulOptions = F_WSMP_NO_TRUNCATION;
    m_rWSMP.cSampleLoops = 0;
    
	m_rWLOOP.cbSize = sizeof(WLOOP);
    m_rWLOOP.ulType = WLOOP_TYPE_FORWARD;
    m_rWLOOP.ulStart = 0;
    m_rWLOOP.ulLength = 0;

	ZeroMemory(&m_CompressedWavObj, sizeof(DMUSP_WAVEOBJECT));
	ZeroMemory(&m_OriginalWavObj, sizeof(DMUSP_WAVEOBJECT));
	ZeroMemory(&m_DecompressedWavObj, sizeof(DMUSP_WAVEOBJECT));

	CopyMemory(&m_OriginalWavObj.WaveFormatEx, &m_rWaveformat, sizeof(WAVEFORMATEX));
	
	if(m_pWavePropPgMgr == NULL)
	{
		m_pWavePropPgMgr = new CWavePropPgMgr();
	}
	else
	{
		m_pWavePropPgMgr->AddRef();
	}

	CDLSComponent* pComponent = pNode->GetComponent();
	ASSERT(pComponent);
	IDMUSProdFramework* pIFramework = pComponent->m_pIFramework;
	ASSERT(pIFramework);
	pIFramework->AllocMemoryStream(FT_RUNTIME, GUID_CurrentVersion, &m_pIMemHeaderStream);
}



//////////////////////////////////////////////////////////////////////
//
//	CWave::~CWave()
//
//////////////////////////////////////////////////////////////////////
CWave::~CWave()
{
	ASSERT(m_pWaveNode);
	CDLSComponent* pComponent = m_pWaveNode->GetComponent();
	ASSERT(pComponent);
	IDMUSProdFramework* pIFramework = pComponent->m_pIFramework;

	// If we are in a Property Page Manager we need to remove ourselves before we go away
	IDMUSProdPropSheet *pIPropSheet;
	if(SUCCEEDED(pIFramework->QueryInterface(IID_IDMUSProdPropSheet, (void**) &pIPropSheet)))
	{
		if(pIPropSheet->IsEqualPageManagerObject(this) == S_OK)
		{
			pIPropSheet->RemovePageManagerByObject(this);
		}
		else
		{
			if(m_pWavePropPgMgr)
			{
				m_pWavePropPgMgr->RemoveObject(this);
			}
		}
		pIPropSheet->Release();
	}
	
	if(m_pWavePropPgMgr)
	{
		DWORD refCount = m_pWavePropPgMgr->Release();
		
		if(refCount == 0) 
			m_pWavePropPgMgr = NULL;
	}

	while(m_arrIntermediateFormats.GetSize() > 0)
	{
		delete[] m_arrIntermediateFormats[0];
		m_arrIntermediateFormats.RemoveAt(0);
	}

	DeleteWaveObjectBuffers(&m_CompressedWavObj);
	DeleteWaveObjectBuffers(&m_OriginalWavObj);
	DeleteWaveObjectBuffers(&m_DecompressedWavObj);

	if(m_pDataManager)
	{
		delete m_pDataManager;
		m_pDataManager = NULL;
	}

	if(m_pIMemHeaderStream)
	{
		m_pIMemHeaderStream->Release();
		m_pIMemHeaderStream = NULL;
	}

	if(m_pDummyInstrument)
	{
		m_pDummyInstrument->Release();
		m_pDummyInstrument = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
//
//	CWave ::QueryInterface()
//
//////////////////////////////////////////////////////////////////////
HRESULT CWave::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if(::IsEqualIID(riid, IID_IPersist)
	||::IsEqualIID(riid, IID_IUnknown))
    {
        AddRef();
        *ppvObj = (IPersist*)this;
        return S_OK;
    }

    if(::IsEqualIID(riid, IID_IPersistStream))
    {
        AddRef();
        *ppvObj = (IPersistStream*)this;
        return S_OK;
    } 

	if( ::IsEqualIID(riid, IID_IDMUSProdPropPageObject) )
    {
        AddRef();
        *ppvObj = (IDMUSProdPropPageObject*)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

//////////////////////////////////////////////////////////////////////
//
//	CWave::AddRef()
//
//////////////////////////////////////////////////////////////////////
ULONG CWave::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	AfxOleLockApp();
    return InterlockedIncrement(&m_lRef);
}

//////////////////////////////////////////////////////////////////////
//
//	CWave::Release()
//
//////////////////////////////////////////////////////////////////////
ULONG CWave::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT(m_lRef != 0);

	InterlockedDecrement(&m_lRef); 

    if(m_lRef == 0)
    {
        delete this;
        return 0;
    }
	AfxOleUnlockApp();
    return m_lRef;
}


/////////////////////////////////////////////////////////////////////////////
// CWave IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CWave IDMUSProdPropPageObject::GetData

HRESULT CWave::GetData(void** ppData)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(ppData);

	*ppData = this;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CWave IDMUSProdPropPageObject::SetData

HRESULT CWave::SetData(void* pData)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pData);

	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CWave IDMUSProdPropPageObject::OnRemoveFromPageManager

HRESULT CWave::OnRemoveFromPageManager(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CWave IDMUSProdPropPageObject::OnShowProperties

HRESULT CWave::OnShowProperties(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	HRESULT hr = E_FAIL;
	ASSERT(m_pWaveNode);

	CDLSComponent* pComponent = m_pWaveNode->GetComponent();
	ASSERT(pComponent);
	IDMUSProdFramework* pIFramework = pComponent->m_pIFramework;


	// Show the Collection properties
	IDMUSProdPropSheet* pIPropSheet;

	hr = pIFramework->QueryInterface(IID_IDMUSProdPropSheet, (void**)&pIPropSheet);
	if(SUCCEEDED(hr))
	{
		int nActivePage = (int) CWavePropPgMgr::dwLastSelPage;
		hr = pIPropSheet->SetPageManager(m_pWavePropPgMgr);
		
		if(SUCCEEDED(hr))
		{
			m_pWavePropPgMgr->SetObject(this);
			pIPropSheet->SetActivePage(short(nActivePage)); 
		}

		pIPropSheet->Show(TRUE);
		pIPropSheet->Release();
	}

	// Set the wave node as the selecetd node if it's not already selected
	m_pWaveNode->SetSelected();

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CWave IPersist implementation

HRESULT CWave::GetClassID(CLSID* pClsId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT(pClsId != NULL);

    memset(pClsId, 0, sizeof( CLSID ));

    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CWave IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
//
// CWave IPersistStream::IsDirty
//
//////////////////////////////////////////////////////////////////////
HRESULT CWave::IsDirty()
{
    // I want to know if I am called
	ASSERT(FALSE);

	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CWave IPersistStream::GetSizeMax
//
//////////////////////////////////////////////////////////////////////
HRESULT CWave::GetSizeMax( ULARGE_INTEGER FAR* /*pcbSize*/ )
{
    // I want to know if I am called
	ASSERT(FALSE);

	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CWave IPersistStream::Load
//
//////////////////////////////////////////////////////////////////////
HRESULT CWave::Load(IStream* pIStream)
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( pIStream != NULL );
	HRESULT hr = E_FAIL;
	MMCKINFO ckMain;
    IDMUSProdRIFFStream* pRiffStream = NULL;

    hr = AllocRIFFStream(pIStream, &pRiffStream);                
	if (SUCCEEDED(hr))
	{
		ckMain.fccType = mmioFOURCC('W','A','V','E');
		if( pRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
		{
			hr = Load(pRiffStream, &ckMain);  
		}
    }

    if (pRiffStream)
    {
        pRiffStream->Release();
    }

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
//
// CWave IPersistStream::Save
//
//////////////////////////////////////////////////////////////////////
HRESULT CWave::Save(IStream* pIStream, BOOL fClearDirty)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT(pIStream);
    
    HRESULT hr = E_FAIL;

    IDMUSProdRIFFStream* pRiffStream = NULL;

    hr = AllocRIFFStream(pIStream, &pRiffStream);                
	if (SUCCEEDED(hr))
	{
        hr = Save(pRiffStream, MMIO_CREATERIFF, TRUE);  //fullsave is true.
    }

    if (pRiffStream)
    {
        pRiffStream->Release();
    }

    if(m_pCollection && fClearDirty )
    {
        m_pCollection->ClearDirtyFlag();
    }

    return hr;

}

HRESULT CWave::Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain)
{
	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_OUTOFMEMORY;
	}

	// Remember where the stream pointer is positioned
	DWORD dwStreamPosition = StreamTell(pIStream);

	// Load the header first
	if(FAILED(LoadHeader(pIRiffStream, pckMain)))
	{
		pIStream->Release();
		return E_FAIL;
	}

	// Load the data
	StreamSeek(pIStream, dwStreamPosition, STREAM_SEEK_SET );

	// Initialize the data manager
	ASSERT(m_pDataManager);
	if(FAILED(m_pDataManager->Initialize(pIStream)))
	{
		TRACE("--- DataManager::Initialize Failed ---\n");
		pIStream->Release();
		return E_FAIL;
	}

	pIStream->Release();

	// Always give precedence to the legacy RSMPL loop structure
	// Otherwise we might end up ignoring changes made to the wave 
	// files in SoundForge etc. which also have WSMPL chunks in them
	if (m_fReadSMPL)
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
	{
		m_bPlayLooped = true;
	}
	else
	{
		m_bPlayLooped = false;
	}

	// Validate the loop values...
	// These might have been set to illegal values outside Producer
	if(m_rWLOOP.ulStart > m_dwWaveLength)
	{
		m_rWLOOP.ulStart = 0;
	}
	
	if(m_rWLOOP.ulStart + m_rWLOOP.ulLength > m_dwWaveLength || m_rWLOOP.ulStart + m_rWLOOP.ulLength < MINIMUM_VALID_LOOPLENGTH)
	{
		m_rWLOOP.ulLength = m_dwWaveLength - m_rWLOOP.ulStart;
	}

	// The compressed loop also gets modified
	RememberLoopBeforeCompression();
	RememberLoopAfterCompression();

	return S_OK;
}

// Loads all the information about the wave except the actual wave data
HRESULT CWave::LoadHeader(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain)
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

	m_bLoadingRuntime = true;

	IDMUSProdPersistInfo* pIJazzPersistInfo;
	if(SUCCEEDED(pIStream->QueryInterface(IID_IDMUSProdPersistInfo,(void**) &pIJazzPersistInfo)))
	{
		DMUSProdStreamInfo	StreamInfo;
		FileType	ftSaveType;

		pIJazzPersistInfo->GetStreamInfo(&StreamInfo);
		ftSaveType = StreamInfo.ftFileType;

		if(ftSaveType == FT_RUNTIME || ftSaveType == FT_UNKNOWN)
		{
			m_bLoadingRuntime = true;
		}
		else
		{
			m_bLoadingRuntime = false;
		}
		
		pIJazzPersistInfo->Release();		
	}

	bool bReadDMUSWaveHeader = false;

	m_fReadFormat = FALSE;
	m_fReadWSMP = FALSE; 
	m_fReadSMPL = FALSE; 
	
	// Clean the wave objects
	CleanWaveObject(&m_OriginalWavObj);
	CleanWaveObject(&m_CompressedWavObj);
	CleanWaveObject(&m_DecompressedWavObj);

	// Walk down the RIFF file looking for chunks
	MMCKINFO ck;
	ck.ckid = 0;
	ck.fccType = 0;
	while(pIRiffStream->Descend(&ck, pckMain, 0) == 0)
	{
		switch(ck.ckid)
		{
			// Load the DRAG chunk
			case FOURCC_DRAG: 
			{
				if(FAILED(ReadDRAG(pIStream, &ck)))
				{
					pIStream->Release();
					return E_FAIL;
				}
				break;
			}

			case DMUS_FOURCC_GUID_CHUNK:
			{
				if(FAILED(ReadGUID(pIStream, &ck)))
				{
					pIStream->Release();
					return E_FAIL;
				}

				break;
			}

			// Load the stereo wave info chunk
			case FOURCC_WVST:
			{
				if(FAILED(ReadWVST(pIStream, &ck)))
				{
					pIStream->Release();
					return E_FAIL;
				}
				break;
			}

			case DMUS_FOURCC_WAVEHEADER_CHUNK:
			{
				_DMUS_IO_WAVE_HEADER waveHeader;
				
				DWORD cb = 0;
				HRESULT hr = pIStream->Read(&waveHeader, sizeof(_DMUS_IO_WAVE_HEADER), &cb) ;
				if(FAILED(hr) || cb != sizeof(_DMUS_IO_WAVE_HEADER))
				{
					return hr = E_FAIL;
				}

				m_bStreaming = waveHeader.dwFlags & DMUS_WAVEF_STREAMING;
				m_bNoPreroll = waveHeader.dwFlags & DMUS_WAVEF_NOPREROLL;
				m_dwReadAheadTime = (DWORD)waveHeader.rtReadAhead / 10000;
				if(m_dwReadAheadTime < MIN_READAHEAD)
				{
					m_dwReadAheadTime = MIN_READAHEAD;
				}
				else if(m_dwReadAheadTime > MAX_READAHEAD)
				{
					m_dwReadAheadTime = MAX_READAHEAD;
				}

				bReadDMUSWaveHeader = true;

				break;
			}

			// Load the chunk with compression information
			case FOURCC_WAVU:
			{
				if(FAILED(ReadWAVU(pIStream, &ck)))
				{
					pIStream->Release();
					return E_FAIL;
				}
				break;
			}

			// RIFF LIST chunk...could be info or an nunknown chunk
			case FOURCC_LIST:
			{
				if(ck.fccType == FOURCC_INFO)
				{
					m_Info.Load(pIRiffStream, &ck);
				}
				else
				{
					// Could be a corrupt RIFF file...
                    // We'll just read the entire LIST chunk
                    DWORD dwStreamPosition = StreamTell(pIStream);
                    m_UnknownChunk.Load(pIRiffStream, &ck, TRUE);
                    StreamSeek(pIStream, dwStreamPosition, STREAM_SEEK_SET );

				}

				break;
			}

			// Load the wave format chunk
			case FOURCC_FMT:
			{
				if(FAILED(ReadFMT(pIStream, &ck)))
				{
					pIStream->Release();
					return E_FAIL;
				}
				m_fReadFormat = TRUE;
				break;
			}

			// Read the WSMPL chunk
			case FOURCC_WSMP:
			{
				if(FAILED(ReadWSMP(pIStream, &ck)))
				{
					pIStream->Release();
					return E_FAIL;
				}

				m_fReadWSMP = TRUE;
				break;
			}

			// Read the RSMPL chunk
			case FOURCC_SMPL:
			{
				if(FAILED(ReadSMPL(pIStream, &ck)))
				{
					pIStream->Release();
					return E_FAIL;
				}

				m_fReadSMPL = TRUE;
				break;
			}

			// Read the data chunk size but NOT the actual data
			case FOURCC_DATA:
			{
				m_dwDataOffset = StreamTell(pIStream);
				if(FAILED(ReadDataInfo(pIStream, &ck)))
				{
					pIStream->Release();
					return E_FAIL;
				}
				break;
			}

			// Ignore these chunks
			case FOURCC_EDIT:
			case FOURCC_FACT:
			{
				break;
			}

			// Whatever other chunks get loaded as unknown chunks
			default:
			{
				m_UnknownChunk.Load(pIRiffStream, &ck, FALSE);
				break;
			}
		}

		pIRiffStream->Ascend(&ck, 0);
		ck.ckid = 0;
		ck.fccType = 0;
	}

	pIStream->Release();

	if(bReadDMUSWaveHeader == false)
	{
		CheckSizeAndSetStreaming();
	}

	return S_OK;
}

void CWave::CheckSizeAndSetStreaming()
{
	UINT nSampleSize = m_rWaveformat.wBitsPerSample / 8;
	DWORD dwWaveLength = m_dwDataSize / nSampleSize;
	DWORD dwPlayTime = dwWaveLength / m_rWaveformat.nSamplesPerSec;

	// Can't set this on a wave in a collection
	// Shouldn't even get here
	if(m_pWaveNode->IsInACollection())
	{
		return;
	}
	if(dwPlayTime > 5)
	{
		m_bStreaming = TRUE;
		m_dwReadAheadTime = 500;
	}
}


HRESULT CWave::WriteGUIDChunk(IDMUSProdRIFFStream* pIRiffStream)
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

	// Write the DMUS_FOURCC_GUID_CHUNK
	MMCKINFO ck;
	ck.ckid = DMUS_FOURCC_GUID_CHUNK;
	HRESULT hr = pIRiffStream->CreateChunk(&ck, 0);
	if(FAILED(hr))
	{
		pIStream->Release();
		return E_FAIL;
	}

	DWORD cb = 0;
	GUID guidFile = m_pWaveNode->GetFileGUID();
	hr = pIStream->Write((LPSTR)&guidFile, sizeof(GUID), &cb);
	if(FAILED(hr))
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

HRESULT CWave::ReadDRAG(IStream* pIStream, MMCKINFO* pckMain)
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
	HRESULT hr = pIStream->Read(&m_dwPreviousRegionLinkID, sizeof(DWORD), &cb) ;
	if(FAILED(hr) || cb != sizeof(DWORD))
	{
		return E_FAIL;
	}

	return hr;
}


HRESULT CWave::ReadGUID(IStream* pIStream, MMCKINFO* pckMain)
{
	ASSERT(m_pWaveNode);
	if(m_pWaveNode == NULL)
	{
		return E_UNEXPECTED;
	}

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
	HRESULT hr = pIStream->Read(&m_pWaveNode->m_guidFile, sizeof(GUID), &cb);
	if(FAILED(hr) || cb != sizeof(GUID))
	{
		return E_FAIL;
	}

	// This will happen when the node gets added to the nodes list in CDLSComponent
	// LoadRIFFChunk doesn't necessarily add the node to the list and other componenets
	// may want to check for duplicate nodes (ContainerDesigner)
	// Fixed for ManBugs 37114
	// m_pWaveNode->SetFileGUID(guidFile);

	return hr;
}


HRESULT CWave::ReadWAVU(IStream* pIStream, MMCKINFO* pckMain)
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
	// Read the flag which indiactes Runtime or Designtime File
	HRESULT hr = pIStream->Read(&m_bLoadingRuntime, sizeof(bool), &cb);
	if(FAILED(hr) || cb != sizeof(bool))
	{
		return E_FAIL;
	}

	// Read the flag that indicates compression
	hr = pIStream->Read(&m_bCompressed, sizeof(bool), &cb);
	if(FAILED(hr) || cb != sizeof(bool))
	{
		return E_FAIL;
	}

	// Init the necessary wave objects
	if(m_bCompressed)
	{
		if(!m_bLoadingRuntime)
		{
			// Read the format we have to compress the wave to...
			hr = pIStream->Read((LPSTR) &(m_CompressedWavObj.WaveFormatEx), sizeof(WAVEFORMATEX), &cb);	
			if(FAILED(hr) || cb != sizeof(WAVEFORMATEX))
			{
				m_bCompressed = false;
				AfxMessageBox(IDS_ERR_UNSUPPORTED_COMPRESSION_FORMAT);
				return E_FAIL;
			}

			// Compressed waves might have extra information in the header
			// Subtract one byte each from the total chunk size for the 
			// runtime/designtime and compressed flags
			ASSERT(pckMain->cksize - 2 >= sizeof(WAVEFORMATEX));
			DWORD cbExtra = m_CompressedWavObj.WaveFormatEx.cbSize;
			if (cbExtra > 0)
			{
				if(FAILED(SafeAllocBuffer(&m_CompressedWavObj.pbExtractWaveFormatData, cbExtra)))
					return E_OUTOFMEMORY;

				hr = pIStream->Read(m_CompressedWavObj.pbExtractWaveFormatData, cbExtra, &cb);
				if(FAILED(hr))
				{
					delete[] m_CompressedWavObj.pbExtractWaveFormatData;
					return E_FAIL;
				}
				ASSERT(cbExtra == cb);
			}
            
            if(pckMain->cksize > sizeof(WAVEFORMATEX) + cbExtra + 2)
            {
                cb = 0;
                hr = pIStream->Read(&m_dwDecompressedStart, sizeof(DWORD), &cb);
                if(FAILED(hr) || cb != sizeof(DWORD))
                {
                    delete[] m_CompressedWavObj.pbExtractWaveFormatData;
                    return E_FAIL;
                }
            }

		}
		else if(m_bLoadingRuntime)
		{
			// Read the format we need to decompress to...
			hr = pIStream->Read((LPSTR) &(m_rWaveformat), sizeof(WAVEFORMATEX), &cb);	
			if(FAILED(hr) || cb != sizeof(WAVEFORMATEX) || m_rWaveformat.wFormatTag != WAVE_FORMAT_PCM)
			{
				AfxMessageBox(IDS_ERR_RUNTIME_UNSUPPORTED_COMPRESSION_FORMAT);
				return E_FAIL;
			}

            if(pckMain->cksize > sizeof(WAVEFORMATEX) + 2)
            {
                cb = 0;
                hr = pIStream->Read(&m_dwDecompressedStart, sizeof(DWORD), &cb);
                if(FAILED(hr) || cb != sizeof(DWORD))
                    return E_FAIL;
            }


            // Make sure the average bytes per sec is correct
            m_rWaveformat.nAvgBytesPerSec = m_rWaveformat.nSamplesPerSec * m_rWaveformat.nBlockAlign;

            CopyMemory(&(m_OriginalWavObj.WaveFormatEx), &m_rWaveformat, sizeof(WAVEFORMATEX));

            m_bUseFormatSuggest = FALSE;
		}
	}

	return S_OK;

}


HRESULT CWave::ReadFMT(IStream* pIStream, MMCKINFO* pckMain)
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

	DWORD cSize = sizeof(WAVEFORMATEX);
	if( pckMain->cksize < sizeof(WAVEFORMATEX) )
	{
		cSize = pckMain->cksize;
	}
	
	DWORD cb = 0;
    WAVEFORMATEX wfxTemp;
	HRESULT hr = pIStream->Read( &wfxTemp, cSize, &cb );
	if(FAILED(hr))
	{
		return E_FAIL;
	}

	// Check for the sample sizes
	if(wfxTemp.wFormatTag == 1 && wfxTemp.wBitsPerSample != 8 && wfxTemp.wBitsPerSample != 16)
	{
		AfxMessageBox(IDS_ERR_WRONGSAMPLESIZE, MB_ICONEXCLAMATION);
		return E_FAIL;
	}

	// Just warn for the sample rates not between 3KHz and 80KHz
	if(wfxTemp.nSamplesPerSec < 3072 || wfxTemp.nSamplesPerSec > 81920)
	{
		AfxMessageBox(IDS_WARN_SAMPLERATE, MB_ICONEXCLAMATION);
	}


    // Is this in PCM format?
	if(wfxTemp.wFormatTag == WAVE_FORMAT_PCM) 
	{
        // Make sure we have the correct average bytes per sec
        wfxTemp.nAvgBytesPerSec = wfxTemp.nSamplesPerSec * wfxTemp.nBlockAlign;

		CopyMemory(&m_rWaveformat, &wfxTemp, sizeof(wfxTemp));
		
		// Copy this into the Original Wav Object
		CopyMemory(&(m_OriginalWavObj.WaveFormatEx), &wfxTemp, sizeof(wfxTemp));

		// If WAVE_FORMAT_PCM the extra info size should be zero
		m_rWaveformat.cbSize = 0;
		m_OriginalWavObj.WaveFormatEx.cbSize = 0;

		return S_OK;
	}
	else if(m_bLoadingRuntime)
	{
		// this muct be a wave file compressed outside of Producer?
		m_bCompressed = true;
	}

	// If it's a compressed runtime file, initialize the CompressedWavObj
	if(m_bCompressed && m_bLoadingRuntime)
	{
		CopyMemory(&(m_CompressedWavObj.WaveFormatEx), &wfxTemp, sizeof(wfxTemp));

		// Now we need the extra info bits
		if(m_CompressedWavObj.WaveFormatEx.cbSize)
		{
			if(m_CompressedWavObj.pbExtractWaveFormatData)
			{
				delete[] m_CompressedWavObj.pbExtractWaveFormatData;
				m_CompressedWavObj.pbExtractWaveFormatData = NULL;
			}

			if(FAILED(SafeAllocBuffer(&m_CompressedWavObj.pbExtractWaveFormatData, m_CompressedWavObj.WaveFormatEx.cbSize)))
			{
				return E_OUTOFMEMORY;
			}

			hr = pIStream->Read(m_CompressedWavObj.pbExtractWaveFormatData, wfxTemp.cbSize, &cb);
			if(FAILED(hr) || cb != wfxTemp.cbSize)
			{
				delete[] m_CompressedWavObj.pbExtractWaveFormatData;
				return E_FAIL;
			}
		}

		if(m_bUseFormatSuggest == TRUE)
		{
			BYTE* pbSrcFormat = NULL;
			if(FAILED(SafeAllocBuffer(&pbSrcFormat, sizeof(WAVEFORMATEX) + wfxTemp.cbSize)))
			{
				return E_OUTOFMEMORY;
			}
			CopyMemory(pbSrcFormat, &m_CompressedWavObj.WaveFormatEx, sizeof(WAVEFORMATEX));
			CopyMemory(((BYTE*)pbSrcFormat) + sizeof(WAVEFORMATEX), m_CompressedWavObj.pbExtractWaveFormatData, wfxTemp.cbSize);
			
			if(MMSYSERR_NOERROR != acmFormatSuggest(NULL, ((WAVEFORMATEX*)pbSrcFormat), &m_rWaveformat, sizeof(WAVEFORMATEX), ACM_FORMATSUGGESTF_WFORMATTAG))
			{
				delete[] pbSrcFormat;
				return E_FAIL;
			}

            CopyMemory(&(m_OriginalWavObj.WaveFormatEx), &m_rWaveformat, sizeof(WAVEFORMATEX));

            delete[] pbSrcFormat;
		}
	}
	else 
	{
		AfxMessageBox(IDS_WAVE_NOT_PCM, MB_ICONEXCLAMATION); 
		return E_FAIL;
	}

	return S_OK;

}


HRESULT CWave::ReadWSMP(IStream* pIStream, MMCKINFO* pckMain)
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

	DWORD cSize = sizeof(WSMPL) + sizeof(WLOOP);
	if(pckMain->cksize < (sizeof(WSMPL) + sizeof(WLOOP)))
	{
		cSize = pckMain->cksize;
	}

	DWORD cb = 0;
	HRESULT hr = pIStream->Read(&m_rWSMP, cSize, &cb);
	if(FAILED(hr) || cb != cSize)
	{
		return E_FAIL;
	}

	return S_OK;
}



HRESULT CWave::ReadSMPL(IStream* pIStream, MMCKINFO* pckMain)
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

	DWORD cSize = (sizeof(RSMPL) + sizeof(RLOOP));
	if(pckMain->cksize < (sizeof(RSMPL) + sizeof(RLOOP)))
	{
		cSize = pckMain->cksize;
	}
    
	DWORD cb = 0;
	HRESULT hr = pIStream->Read( &m_rRSMPL, cSize, &cb );
	if(FAILED(hr) || cb != cSize)
	{
		return E_FAIL;
	}

	return S_OK;

}


HRESULT CWave::ReadDataInfo(IStream* pIStream, MMCKINFO* pckMain)
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

	// Get the data size but the wavelength calculation should happen in the 
	// derived objects depending on whether the wave is mono or multichannel
	if(m_bCompressed && m_bLoadingRuntime)
	{
		m_CompressedWavObj.dwDataSize = pckMain->cksize;
	}
	else
	{
		m_dwDataSize = pckMain->cksize;
		m_OriginalWavObj.dwDataSize = m_dwDataSize;
	}

	return S_OK;
}

HRESULT CWave::ReadData(IStream* pIStream, MMCKINFO* pckMain, DWORD dwOffset, DWORD dwLength, BYTE* pbData)
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

	ASSERT(pbData);
	if(pbData == NULL)
	{
		return E_INVALIDARG;
	}

	DWORD cb = 0;
	StreamSeek(pIStream, dwOffset, STREAM_SEEK_SET);
	HRESULT hr = pIStream->Read(pbData, dwLength, &cb);
	if(FAILED(hr) || cb != dwLength)
	{
		return E_FAIL;
	}

	return S_OK;	
}


//////////////////////////////////////////////////////////////////////
//
//	CWave::CopySMPLToWSMP()
//
//////////////////////////////////////////////////////////////////////
void CWave::CopySMPLToWSMP()
{
    DWORD dwTemp;
    m_rWSMP.usUnityNote = (USHORT) m_rRSMPL.dwMIDIUnityNote;
    dwTemp = m_rRSMPL.dwMIDIPitchFraction >> 16;
    dwTemp *= 100;
    dwTemp >>= 16;
    m_rWSMP.sFineTune = (short) dwTemp;
    m_rWSMP.cSampleLoops = m_rRSMPL.cSampleLoops;
    m_rWLOOP.ulStart = m_rRLOOP.dwStart;
    m_rWLOOP.ulLength = (m_rRLOOP.dwEnd + 1) - m_rRLOOP.dwStart;
    m_rWLOOP.ulType = m_rRLOOP.dwType;
}

//////////////////////////////////////////////////////////////////////
//
//	CWave::CopyWSMPToSMPL()
//
//////////////////////////////////////////////////////////////////////
void CWave::CopyWSMPToSMPL()
{
    long lTemp = m_rWSMP.sFineTune;
	m_rRSMPL.dwMIDIUnityNote = m_rWSMP.usUnityNote;
	
	// dwMIDIPitchFraction is always +ve
	while (lTemp < 0)
	{
		m_rRSMPL.dwMIDIUnityNote--;
		lTemp += 100;
	}
	
	while(lTemp >= 100)
	{
		m_rRSMPL.dwMIDIUnityNote++;
		lTemp -= 100;
	}

    lTemp <<= 16;
    lTemp /= 100;
    m_rRSMPL.dwMIDIPitchFraction = (DWORD) lTemp << 16;
    m_rRSMPL.cSampleLoops = m_rWSMP.cSampleLoops;
    m_rRLOOP.dwStart = m_rWLOOP.ulStart;
    m_rRLOOP.dwEnd = m_rWLOOP.ulStart + m_rWLOOP.ulLength - 1;
    m_rRLOOP.dwType = m_rWLOOP.ulType;    
}



HRESULT CWave::PromptForSaveFileName(CString& sSaveFileName, CString& sSavePath)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWaveNode);
	if(m_pWaveNode == NULL)
		return E_FAIL;

	// Open the saveas dialog
	HRESULT		hr = S_OK;
    CHAR szFilter[30] = "Wave Files (*.wav)|*.wav||";
    CFileDialog FileDlg(FALSE, "WAV", GetName().GetBuffer(0), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, NULL);

    if (FileDlg.DoModal() == IDOK)
    {
        sSaveFileName = FileDlg.GetFileName();
		sSavePath = FileDlg.GetPathName();
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CWave::SaveAs(IStream* pIStream, bool bPromptForFileName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWaveNode);
	if(m_pWaveNode == NULL)
		return E_FAIL;

	// We need to get a stream for a file if the passed stream is NULL
	if(pIStream == NULL && bPromptForFileName == false)
	{
		return E_FAIL;
	}
	
	// We don't need to prompt for a filename if a valid stream is passed 
	if(pIStream && bPromptForFileName)
	{
		return E_FAIL;
	}


	CDLSComponent* pComponent = m_pWaveNode->GetComponent();
	ASSERT(pComponent);
	if(pComponent == NULL)
		return E_FAIL;

	IDMUSProdFramework* pIFramework = pComponent->m_pIFramework;
	if(pIFramework == NULL)
		return E_FAIL;


	HRESULT hr = E_FAIL;
	CString sSavePath = "";

	if(bPromptForFileName)
	{
		// Open the saveas dialog
		CString sSaveFileName = "";
		if(SUCCEEDED(PromptForSaveFileName(sSaveFileName, sSavePath)))
		{
			hr = AllocStreamToFile(&pIStream, sSaveFileName.GetBuffer(0), 0);
			sSaveFileName.ReleaseBuffer();
		}
		else
		{
			return hr;
		}
	}

	if(pIStream)
	{
		// Save out the current wave as a .wav file on disk.
		// Turn of compression flag for save. We will always
		// save compressed if it is compressed.
		bool bNeedToSetCompression = false;

		if(SUCCEEDED(hr = SaveAs(pIStream)))
		{
			if(!sSavePath.IsEmpty())
			{
				// Release the stream so it may be used again.
				if(bPromptForFileName && pIStream)
				{
					pIStream->Release();
				}

				BSTR bstrFileName = sSavePath.AllocSysString();
				pIFramework->ShowFile(bstrFileName);
			}
		}
		
		if(bNeedToSetCompression)
			m_bCompressed = true;

	}

	return hr;
}

HRESULT CWave::SaveAs(IStream* pIStream)
{
	ASSERT(pIStream);
	if(pIStream == NULL)
		return E_POINTER;

	HRESULT hr = E_FAIL;

	if(pIStream)
	{
		hr = Save(pIStream, FALSE);
	}
	
	return hr;
}


HRESULT CWave::WriteDataToStream(IStream* pIStream, DWORD dwStartSample, DWORD dwLength, DWORD& dwSamplesWritten)
{
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_POINTER;
	}

	UINT nSampleSize = m_rWaveformat.wBitsPerSample / 8;
	UINT nChannels = m_rWaveformat.nChannels;
		
	// The data range could be huge so we need to do this in chunks of 4K
	DWORD dwChunkSize = 4096;	
	dwSamplesWritten = 0;
	DWORD dwChunkStart = dwStartSample;
	DWORD dwSamplesToWrite = dwLength;
	DWORD dwBytesWritten = 0;

	while((int)dwSamplesToWrite > 0)
	{
		BYTE* pbData = NULL;
		DWORD cbRead = 0;
		if(FAILED(GetData(dwChunkStart, dwSamplesToWrite, &pbData, cbRead)))
		{
			return E_FAIL;
		}
		
		// Write it to IStream
		DWORD cbWritten = 0;
		HRESULT hr = pIStream->Write((LPSTR)pbData, cbRead, &cbWritten);
		if(FAILED(hr) || cbWritten != cbRead)
		{
			delete[] pbData;
			return E_FAIL;
		}

		delete[] pbData;
		dwSamplesWritten += cbWritten / nSampleSize / nChannels;
		dwSamplesToWrite -= dwSamplesWritten;
		dwChunkStart += dwSamplesWritten;
		dwBytesWritten += cbWritten;
	}

	return S_OK;
}

void CWave::SetName(CString sName)
{
	LONG cChannels = GetChannelCount();
	LONG iChannel;

	// set name of wave and all channels
	m_Info.m_csName = sName;
	if (cChannels > 1)
		for (iChannel = 0; iChannel < cChannels; iChannel++)
			GetChannel(iChannel)->m_Info.m_csName = sName;

	// update the instrument and instrument editor if we're in a collection
	if(m_pCollection)
	{
		CCollectionInstruments* plstInstruments = m_pCollection->GetInstruments();
		CInstrument* pInstrument = plstInstruments->GetHead();
		while (pInstrument) // for all instruments go thru all regions
		{
			CInstrumentRegions* plstRegions = pInstrument->GetRegions();
			CRegion* pRegion = plstRegions->GetHead();
			while (pRegion)
			{
				if (IsReferencedBy(pRegion->m_pWave))
					{
					pInstrument->RefreshUI(true);
					break; // no need to go over other regions, we updated the whole instrument
					}
				
				pRegion = pRegion->GetNext();
			}
			pInstrument = pInstrument->GetNext();
		}
	}

	ASSERT(m_pWaveNode);
	CDLSComponent* pComponent = m_pWaveNode->GetComponent();
	ASSERT(pComponent);
	IDMUSProdFramework* pIFramework = pComponent->m_pIFramework;

	// update the property page if it's showing
	if(m_pWavePropPgMgr)
	{
		IDMUSProdPropSheet* pIPropSheet;
		if(SUCCEEDED(pIFramework->QueryInterface(IID_IDMUSProdPropSheet, (void**)&pIPropSheet)))
		{
			if(pIPropSheet->IsShowing() == S_OK && pIPropSheet->IsEqualPageManagerObject(this) == S_OK)
			{
				m_pWavePropPgMgr->SetObject(this);
			}
			RELEASE(pIPropSheet);
		}
	}
}


void CWave::RefreshPropertyPage()
{
	ASSERT(m_pWaveNode);
	CDLSComponent* pComponent = m_pWaveNode->GetComponent();
	ASSERT(pComponent);

	IDMUSProdPropSheet* pIPropSheet = NULL;

	if(SUCCEEDED(pComponent->m_pIFramework->QueryInterface(IID_IDMUSProdPropSheet, (void**)&pIPropSheet)))
	{
		if(pIPropSheet->IsShowing() == S_OK && pIPropSheet->IsEqualPageManagerObject(this) == S_OK)
		{
			// Change the wave property page if it's up.
			if (m_pWavePropPgMgr)
				m_pWavePropPgMgr->RefreshData();
			
			// Update the loop points for the popertypage
			CWavePropPg* pWavePropPg = m_pWavePropPgMgr->m_pWavePage;
			CWaveInfoPropPg* pWaveInfoPropPg = m_pWavePropPgMgr->m_pWaveInfoPage;
			CWaveCompressionPropPage* pWaveCompressionPropPg = m_pWavePropPgMgr->m_pWaveCompressionPage;

			DWORD dwLastSelPage = m_pWavePropPgMgr->dwLastSelPage;

			if(dwLastSelPage == WAVE_PROP_PAGE)
				pWavePropPg->InitializeDialogValues();
			else if(dwLastSelPage == WAVE_INFO_PAGE)
				pWaveInfoPropPg->InitializeDialogValues();
			else if(dwLastSelPage == WAVE_COMPRESSION_PAGE)
				pWaveCompressionPropPg->InitializeDialogValues();
		}
		
		RELEASE(pIPropSheet);
	}
	
}

void CWave::Replace()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pWaveNode);
	if(m_pWaveNode == NULL)
		return;

	m_pWaveNode->Stop( TRUE );

	if(m_pCollection != NULL)
		{
		// replace in a DLS collection

		// Put up a file open dilaog.
		CString sFileName;
		CHAR szFilter[39] = "Wave Files (*.wav;*.wvp)|*.wav;*.wvp||";
		CFileDialog fileDlg(TRUE, "WAV", NULL, OFN_FILEMUSTEXIST | OFN_ENABLESIZING , szFilter, NULL);
		
		if(fileDlg.DoModal() == IDOK)
			{
			sFileName = fileDlg.GetFileName();
			if(sFileName.IsEmpty())
				return;

			m_pCollection->m_Waves.ReplaceWave(m_pWaveNode, sFileName);
			}
		}
	else
		{
		// replace outside of a DLS collection
		IDMUSProdDocType*	pIDocType;
		IDMUSProdNode*		pINewWaveNode;

		// get framework
		IDMUSProdFramework* pIFramework = theApp.m_pFramework;
		if (pIFramework == NULL)
			{
			ASSERT(FALSE); // no framework
			return;
			}

		// Get DocType for DLS collections
		if (FAILED(pIFramework->FindDocTypeByNodeId(GUID_WaveNode, &pIDocType)))
			return;

		// load File Open dialog prompt
		CString strOpenDlgTitle;
		strOpenDlgTitle.LoadString( IDS_FILE_OPEN_ANY_WAVE );

		// Display File open dialog
		if( pIFramework->OpenFile(pIDocType, strOpenDlgTitle.AllocSysString(), m_pWaveNode, &pINewWaveNode) != S_OK )
			{
			// Did not open a file, or opened file other than wave file
			if( pINewWaveNode )
				pINewWaveNode->Release();
			return;
			}

		if (pIDocType)
			pIDocType->Release();

		// update all references to point to the new file
		pIFramework->NotifyNodes(m_pWaveNode, FRAMEWORK_FileReplaced, pINewWaveNode);

		// tree is now holding on to new node
		pINewWaveNode->Release();
					
		// delete the old file
		m_pWaveNode->DeleteNode(FALSE);
		}
}

CString CWave::GetName()
{
	return m_Info.m_csName;
}


void CWave::SetCollection(CCollection* pCollection)
{
	// We don't check for the validity of the pointer 
	// because we might want to set the collection to NULL
	m_pCollection = pCollection;
}

CCollection* CWave::GetCollection() const
{
	return m_pCollection;
}


BOOL CWave::WaveIsPlaying()
{
	return m_bIsPlaying;
}


CWaveNode* CWave::GetNode() const
{
	return m_pWaveNode;
}

CWaveCtrl* CWave::GetWaveEditor() const
{
	ASSERT(m_pWaveNode);
	if(m_pWaveNode == NULL)
		return NULL;

	return m_pWaveNode->GetWaveEditor();
}

bool CWave::IsStereo()
{
	return m_pWaveNode->IsStereo();
}

HRESULT CWave::SafeAllocBuffer(BYTE** ppbBuffer, DWORD dwSize)
{
	BYTE* pbTemp = new BYTE[dwSize];
	if(pbTemp == NULL)
	{
		return E_OUTOFMEMORY;
	}

	ZeroMemory(pbTemp, dwSize);
	*ppbBuffer = pbTemp;

	return S_OK;
}

int CWave::GetNumberOfLoops()
{
	return m_rWSMP.cSampleLoops;
}

DWORD CWave::GetWaveLength()
{
	return m_dwWaveLength;
}

bool CWave::IsLooped()
{
	return m_bPlayLooped != 0;
}

void CWave::SetLooped(bool bLoop)
{
	m_bPlayLooped = bLoop;
}

WSMPL CWave::GetWSMPL()
{
	return m_rWSMP;
}

WLOOP CWave::GetWLOOP()
{
	return m_rWLOOP;
}

RSMPL CWave::GetRSMPL()
{
	return m_rRSMPL;
}

RLOOP CWave::GetRLOOP()
{
	return m_rRLOOP;
}

void CWave::SetWSMPL(WSMPL& newWSMPL)
{
	CopyMemory(&m_rWSMP, &newWSMPL, sizeof(WSMPL));
	
	// Always keep RSMPL in sync with WSMPL
	CopyWSMPToSMPL();
}


void CWave::SetWLOOP(WLOOP& newWLOOP)
{
	CopyMemory(&m_rWLOOP, &newWLOOP, sizeof(WLOOP));

	// Always keep WLOOP and RLOOP in sync
	m_rRLOOP.dwStart = m_rWLOOP.ulStart;
    m_rRLOOP.dwEnd = m_rWLOOP.ulStart + m_rWLOOP.ulLength - 1;
    m_rRLOOP.dwType = m_rWLOOP.ulType;    
}


void CWave::SetRSMPL(RSMPL& newRSMPL)
{
	CopyMemory(&m_rRSMPL, &newRSMPL, sizeof(RSMPL));
}


void CWave::SetRLOOP(RLOOP& newRLOOP)
{
	CopyMemory(&m_rRLOOP, &newRLOOP, sizeof(RLOOP));
}


DWORD CWave::GetDataSize()
{
	return m_dwDataSize;
}


HRESULT CWave::SetLoop(DWORD dwStart, DWORD dwEnd)
{
    // The synth deals with the WSMPL and WLOOP values
    // But we want to save the smpl values also
	WSMPL waveWSMPL = GetWSMPL();
	WLOOP waveLoop = GetWLOOP();
	RSMPL waveRSMPL = GetRSMPL();
	RLOOP waveRLoop = GetRLOOP();

    waveWSMPL.cSampleLoops = 1;
    
	waveLoop.ulStart = dwStart;
    waveLoop.ulLength = (dwEnd + 1) - dwStart;

    waveRSMPL.cSampleLoops = 1;
    waveRLoop.dwStart = (DWORD)dwStart;
    waveRLoop.dwEnd = (DWORD)dwEnd;
	
	m_bPlayLooped = true;

	SetWSMPL(waveWSMPL);
	SetWLOOP(waveLoop);
	SetRSMPL(waveRSMPL);
	SetRLOOP(waveRLoop);
	
	// Set the compressed loop points also
	RememberLoopAfterCompression();

	if(FAILED(UpdateWave()))
	{
		return E_FAIL;
	}

	UpdateLoopInRegions();
    UpdateDummyInstrument();

	SetDirtyFlag();

	return S_OK;
}

WAVEFORMATEX* CWave::CopyWaveFormat(const WAVEFORMATEX* pwfxSrc, bool bCompress)
{
	ASSERT(pwfxSrc);
	if(pwfxSrc == NULL)
		return NULL;

	DWORD dwSrcExtraBytes = 0;

#ifdef MSAUDIO
	if(pwfxSrc->wFormatTag == WMAUDIO_FORMAT_TAG || pwfxSrc->wFormatTag == MSAUDIO_FORMAT_TAG)
	{
		dwSrcExtraBytes = sizeof(MSAUDIO_KEY);
	}
#endif
	
	// Make copies of the original formats
	WAVEFORMATEX* pwfxSrcCopy = (WAVEFORMATEX*) new BYTE[sizeof(WAVEFORMATEX) + pwfxSrc->cbSize + dwSrcExtraBytes];
	// Out of memory?
	if(pwfxSrcCopy == NULL)
		return NULL;

	ZeroMemory(pwfxSrcCopy, sizeof(WAVEFORMATEX) + pwfxSrc->cbSize);

	CopyMemory(pwfxSrcCopy, pwfxSrc, sizeof(WAVEFORMATEX) + pwfxSrc->cbSize);

#ifdef MSAUDIO
	if(pwfxSrcCopy->wFormatTag == WMAUDIO_FORMAT_TAG || pwfxSrcCopy->wFormatTag == MSAUDIO_FORMAT_TAG)
	{
		if(bCompress)
			strcpy((char*)(pwfxSrcCopy) + pwfxSrcCopy->cbSize + sizeof(WAVEFORMATEX), MSAUDIO_KEY);
		else
			strcpy((char*)(pwfxSrcCopy) + pwfxSrcCopy->cbSize + sizeof(WAVEFORMATEX), MSAUDIO_DEC_KEY);

		// Ummm...both keys have the same size
		pwfxSrcCopy->cbSize += sizeof(MSAUDIO_KEY);
	}
#endif

	return pwfxSrcCopy;
}


HRESULT CWave::CopyWaveObject(const DMUSP_WAVEOBJECT* pSrcWaveObject, DMUSP_WAVEOBJECT*  pDstWaveObject, DWORD dwDataBufferSize)
{
	ASSERT(pSrcWaveObject);
	if(pSrcWaveObject == NULL)
		return E_FAIL;

	ASSERT(pDstWaveObject);
	if(pDstWaveObject == NULL)
		return E_FAIL;

	if(dwDataBufferSize == 0xFFFFFFFF)
		dwDataBufferSize = pSrcWaveObject->dwDataSize;
	
	CopyMemory(&(pDstWaveObject->WaveFormatEx), &(pSrcWaveObject->WaveFormatEx), sizeof(WAVEFORMATEX));
	
	if(pSrcWaveObject->WaveFormatEx.cbSize && pSrcWaveObject->pbExtractWaveFormatData)
	{
		pDstWaveObject->pbExtractWaveFormatData = new BYTE[pSrcWaveObject->WaveFormatEx.cbSize];
		
		// Out of memory??
		if(pDstWaveObject->pbExtractWaveFormatData == NULL)
		{
			return E_FAIL;
		}
			
		ZeroMemory(((BYTE*)pDstWaveObject->pbExtractWaveFormatData), pSrcWaveObject->WaveFormatEx.cbSize);
		CopyMemory(((BYTE*)pDstWaveObject->pbExtractWaveFormatData), ((BYTE*)pSrcWaveObject->pbExtractWaveFormatData), pSrcWaveObject->WaveFormatEx.cbSize);
	}

	if(pSrcWaveObject->pbData != NULL)
	{
		pDstWaveObject->pbData = new BYTE[dwDataBufferSize];
		
		// Out of memory??
		if(pDstWaveObject->pbData == NULL)
		{
			if(pDstWaveObject->pbExtractWaveFormatData)
				delete[] pDstWaveObject->pbExtractWaveFormatData;

			return E_FAIL;
		}

		ZeroMemory(((BYTE*)pDstWaveObject->pbData), dwDataBufferSize);
		CopyMemory(((BYTE*)pDstWaveObject->pbData), ((BYTE*)pSrcWaveObject->pbData), pSrcWaveObject->dwDataSize);

		pDstWaveObject->dwDataSize = dwDataBufferSize;
	}
	else
	{
		pDstWaveObject->pbData = NULL;
		pDstWaveObject->dwDataSize = 0;
	}

	return S_OK;
}

// Deletes the buffers allocated in the WaveObject
void CWave::DeleteWaveObjectBuffers(DMUSP_WAVEOBJECT* pWaveObject)
{
	ASSERT(pWaveObject);
	if(pWaveObject)
	{
		if(pWaveObject->pbData)
		{
			delete[] pWaveObject->pbData;
			pWaveObject->pbData = NULL;
		}
		
		if(pWaveObject->pbExtractWaveFormatData)
		{
			delete[] pWaveObject->pbExtractWaveFormatData;
			pWaveObject->pbExtractWaveFormatData = NULL;
		}
	}
}


void CWave::CleanWaveObject(DMUSP_WAVEOBJECT* pWaveObject)
{
	if(pWaveObject)
	{
		DeleteWaveObjectBuffers(pWaveObject);
		ZeroMemory(pWaveObject, sizeof(DMUSP_WAVEOBJECT));
	}
}


bool CWave::IsUnityNoteAndFractionSame(const RSMPL& rsmpl, short sUnityNote, short sFineTune)
{
    long lTemp = sFineTune;
	DWORD dwMIDIUnityNote = sUnityNote;
	
	// dwMIDIPitchFraction is always +ve
	while (lTemp < 0)
	{
		dwMIDIUnityNote--;
		lTemp += 100;
	}
	
	while(lTemp >= 100)
	{
		dwMIDIUnityNote++;
		lTemp -= 100;
	}

    lTemp <<= 16;
    lTemp /= 100;
    DWORD dwMIDIPitchFraction = (DWORD) lTemp << 16;

	if(rsmpl.dwMIDIUnityNote == dwMIDIUnityNote && rsmpl.dwMIDIPitchFraction == dwMIDIPitchFraction)
		return true;
	else
		return false;
}



// This code was partly lifted and modified from the WMAudio encoder app source...
HRESULT CWave::CalculateConversionSteps(const WAVEFORMATEX* pwfxSrc, const WAVEFORMATEX* pwfxDst, bool bEncode)
{

	ASSERT(pwfxSrc);
	ASSERT(pwfxDst);

	MMRESULT mmr = ACMERR_NOTPOSSIBLE;
	HRESULT hr = E_FAIL;

	// Clean up the previous objects if any left
	while(m_arrIntermediateFormats.GetSize() > 0)
	{
		delete[] m_arrIntermediateFormats[0];
		m_arrIntermediateFormats.RemoveAt(0);
	}

	// Make copies of the original formats
	WAVEFORMATEX* pwfxSrcCopy = CopyWaveFormat(pwfxSrc, bEncode);
	if(pwfxSrcCopy == NULL)
		return E_FAIL;

	WAVEFORMATEX* pwfxDstCopy = CopyWaveFormat(pwfxDst, bEncode);
	if(pwfxDstCopy == NULL)
		return E_FAIL;

    ///////////////////////////////////////////////////////////////////////////////////
    //  BUGBUG!! We're shorting the multistep conversion for DX8 as API can't handle it
    ///////////////////////////////////////////////////////////////////////////////////
    m_arrIntermediateFormats.Add(pwfxSrcCopy);
    m_arrIntermediateFormats.Add(pwfxDstCopy);
    return S_OK;


	// Short-cicuit the mulitstep conversion if we're dealing with the WMAudio codecs
	if((pwfxDstCopy->wFormatTag == WMAUDIO_FORMAT_TAG || pwfxDstCopy->wFormatTag == MSAUDIO_FORMAT_TAG) &&
		pwfxDstCopy->nChannels == pwfxSrcCopy->nChannels &&
		pwfxDstCopy->wBitsPerSample == pwfxSrcCopy->wBitsPerSample)
	{
		pwfxSrcCopy->nSamplesPerSec = pwfxDstCopy->nSamplesPerSec;
		pwfxSrcCopy->nAvgBytesPerSec = pwfxSrcCopy->nSamplesPerSec * pwfxSrcCopy->nBlockAlign;

		m_arrIntermediateFormats.Add(pwfxSrcCopy);
		m_arrIntermediateFormats.Add(pwfxDstCopy);
		return S_OK;
	}

	if((pwfxSrcCopy->wFormatTag == WMAUDIO_FORMAT_TAG || pwfxSrcCopy->wFormatTag == MSAUDIO_FORMAT_TAG) &&
		pwfxDstCopy->nChannels == pwfxSrcCopy->nChannels &&
		pwfxDstCopy->wBitsPerSample == pwfxSrcCopy->wBitsPerSample)
	{
		pwfxDstCopy->nSamplesPerSec = pwfxSrcCopy->nSamplesPerSec;
		pwfxDstCopy->nAvgBytesPerSec = pwfxDstCopy->nSamplesPerSec * pwfxDstCopy->nBlockAlign;

		m_arrIntermediateFormats.Add(pwfxSrcCopy);
		m_arrIntermediateFormats.Add(pwfxDstCopy);
		return S_OK;
	}

	// Determine if we have to massage the input into an intermediate form of PCM
    WAVEFORMATEX* pwfxPCM1 = (WAVEFORMATEX*) new BYTE[sizeof(WAVEFORMATEX)];
	ZeroMemory(pwfxPCM1, sizeof(WAVEFORMATEX));
    pwfxPCM1->wFormatTag = WAVE_FORMAT_PCM;

	mmr = acmFormatSuggest(NULL, pwfxSrcCopy, pwfxPCM1, sizeof(WAVEFORMATEX), ACM_FORMATSUGGESTF_WFORMATTAG);
    if(MMSYSERR_NODRIVER == mmr )
	{
		delete[] pwfxSrcCopy;
		delete[] pwfxDstCopy;
		return E_FAIL;
	}

    // Determine if the compressor can generate requested format without an intermediate step.
    WAVEFORMATEX* pwfxPCM2 = (WAVEFORMATEX*) new BYTE[sizeof(WAVEFORMATEX)];;
	ZeroMemory(pwfxPCM2, sizeof(WAVEFORMATEX));
    pwfxPCM2->wFormatTag = WAVE_FORMAT_PCM;

    mmr = acmFormatSuggest(NULL, pwfxDstCopy, pwfxPCM2, sizeof(WAVEFORMATEX), ACM_FORMATSUGGESTF_WFORMATTAG);
    if(MMSYSERR_NODRIVER == mmr )
    {
		delete[] pwfxSrcCopy;
		delete[] pwfxDstCopy;

        return E_FAIL;
    }

	// First format is the source format
	m_arrIntermediateFormats.Add(pwfxSrcCopy);

    // Figure out how many steps we need
	int nSteps = 1;

    BOOL fInputPCM  = (WAVE_FORMAT_PCM == pwfxSrc->wFormatTag);
    BOOL fOutputPCM = (WAVE_FORMAT_PCM == pwfxDst->wFormatTag);

    BOOL fOutputVox = IsVoxwareCodec(pwfxDst->wFormatTag);
    
    DWORD dwInSampRate = pwfxSrc->nSamplesPerSec;
    if( fOutputVox && pwfxSrc->nChannels == pwfxDst->nChannels &&
        dwInSampRate >= pwfxDst->nSamplesPerSec && (dwInSampRate == 8000 || dwInSampRate == 11025 ||
        dwInSampRate == 16000 || dwInSampRate == 22050 || dwInSampRate == 44100))
    {
        //
        // Voxware can convert directly from any PCM... with a few caveats:
        // 1) The number of channels must match.
        // 2) The output sample rate must be less than or equal to the input
        // sample rate
        //
        if( fInputPCM )
        {
            nSteps = 1;
        }
        else
        {
            nSteps = 2;
        }
    }
    else
    {
        // The input and output is PCM
        if( fInputPCM && fOutputPCM )
		{
            nSteps = 1;

		}
        // Input is PCM and the intermediate output PCM matches it
        else if( fInputPCM  && 0 == memcmp( pwfxSrc, pwfxPCM2, sizeof(PCMWAVEFORMAT) ) )
        {
            nSteps = 1;
        }
        
		// Output is PCM and the intermediate input PCM matches it
        else if( fOutputPCM && 0 == memcmp( pwfxDst, pwfxPCM1, sizeof(PCMWAVEFORMAT) ) )
        {
			// Convert directly from source to destination
            nSteps = 1;                   
        }
        else if( fInputPCM || fOutputPCM )
        {
			// Need one intermediate conversion
            nSteps = 2;                   
        }
        else
        {
			// Slowest conversion possible.
            nSteps = 3;                   
        }
    }

	// Correct ASSUMPTION about single step.    
    switch( nSteps )                  
    {
        case 1:                         // Assumption was correct.
		{
			if(pwfxPCM1)
				delete[] pwfxPCM1;
			
			if(pwfxPCM2)
				delete[] pwfxPCM2;
            break;
		}

        case 2:                         // Insert one intermediate step in between.
        {
			if(fInputPCM)
			{
				m_arrIntermediateFormats.Add(pwfxPCM2);
				delete[] pwfxPCM1;
			}
			else
			{
				m_arrIntermediateFormats.Add(pwfxPCM1);
				delete[] pwfxPCM2;


			}

            break;
        }

        case 3:                         // Add two intermediate step.
		{
            m_arrIntermediateFormats.Add(pwfxPCM1);  // Insert input conversion.
            m_arrIntermediateFormats.Add(pwfxPCM2);	 // Insert the output conversion
            
			break;
		}
    }

	
	// Last format is the desired destination format
	m_arrIntermediateFormats.Add(pwfxDstCopy);


    return S_OK;
}

HRESULT CWave::ConvertWave(const WAVEFORMATEX* pwfxSource, const WAVEFORMATEX* pwfxDest, BYTE* pbSource, const DWORD dwSourceSize, BYTE** ppbDest, DWORD& cbConverted)
{
	ASSERT(pwfxSource);
	if(pwfxSource == NULL)
	{
		return E_POINTER;
	}

	ASSERT(pwfxDest);
	if(pwfxDest == NULL)
	{
		return E_POINTER;
	}

	ASSERT(pbSource);
	if(pbSource == NULL)
	{
		return E_POINTER;
	}

	// Do we have anything to convert?
	if(dwSourceSize <= 0)
	{
		return E_INVALIDARG;
	}

	DMUSP_WAVEOBJECT sourceWave;
	ZeroMemory(&sourceWave, sizeof(DMUSP_WAVEOBJECT));
	sourceWave.dwDataSize = dwSourceSize;
	sourceWave.pbData = pbSource;
	CopyMemory(&sourceWave.WaveFormatEx, pwfxSource, sizeof(WAVEFORMATEX));
	if(pwfxSource->wFormatTag != 1 && pwfxSource->cbSize != 0)
	{
		if(FAILED(SafeAllocBuffer(&(sourceWave.pbExtractWaveFormatData), pwfxSource->cbSize)))
		{
			return E_OUTOFMEMORY;
		}
		CopyMemory(sourceWave.pbExtractWaveFormatData, ((BYTE*)pwfxSource) + sizeof(WAVEFORMATEX), pwfxSource->cbSize);
	}

	DMUSP_WAVEOBJECT destWave;
	ZeroMemory(&destWave, sizeof(DMUSP_WAVEOBJECT));
	
	bool bCompress = pwfxDest->wFormatTag != 1 ? true : false;

	if(FAILED(ConvertWave(sourceWave, pwfxDest, destWave, bCompress)))
	{
		if(sourceWave.pbExtractWaveFormatData)
		{
			delete[] sourceWave.pbExtractWaveFormatData;
			sourceWave.pbExtractWaveFormatData = NULL;
		}

		cbConverted = 0;
		return E_FAIL;
	}

	*ppbDest = destWave.pbData;
	cbConverted = destWave.dwDataSize;

	if(sourceWave.pbExtractWaveFormatData)
	{
		delete[] sourceWave.pbExtractWaveFormatData;
		sourceWave.pbExtractWaveFormatData = NULL;
	}
	
	if(destWave.pbExtractWaveFormatData)
	{
		delete[] destWave.pbExtractWaveFormatData;
		destWave.pbExtractWaveFormatData = NULL;
	}

	return S_OK;
}



HRESULT CWave::ConvertWave(const DMUSP_WAVEOBJECT& sourceWave, const WAVEFORMATEX* pwfxDst, DMUSP_WAVEOBJECT& convertedWaveObj, bool bCompress, DWORD dwSilenceSamples)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(pwfxDst);
	if(pwfxDst == NULL)
		return E_FAIL;

	// Some codecs chop off the end (like MSAudio beta V1 that does not handle the ACM_STREAMCONVERTF_END flag)
	// So we keep track of the lost bytes and append that much silence so we don't lose any real data
	DWORD dwSilenceBytes = dwSilenceSamples;
	if(sourceWave.WaveFormatEx.wBitsPerSample > 8)
		dwSilenceBytes = dwSilenceBytes * 2;

	// Used only to keep track of opened MSAudio Beta V1 driver
	bool bCloseBetaDriver = false;

	DWORD dwExtraBytes = 0;
	if(sourceWave.WaveFormatEx.wFormatTag != WAVE_FORMAT_PCM)
		dwExtraBytes = sourceWave.WaveFormatEx.cbSize;

	WAVEFORMATEX* pwfxSrc = (WAVEFORMATEX*) new BYTE[sizeof(WAVEFORMATEX) + dwExtraBytes];
	
	// Out of memory??
	if(pwfxSrc == NULL)
	{
		return E_FAIL;
	}

	ZeroMemory(pwfxSrc, sizeof(WAVEFORMATEX) + dwExtraBytes);
	CopyMemory(pwfxSrc, &(sourceWave.WaveFormatEx), sizeof(WAVEFORMATEX));
	CopyMemory(((BYTE*)pwfxSrc) + sizeof(WAVEFORMATEX), sourceWave.pbExtractWaveFormatData, dwExtraBytes);  

	DWORD cbMaxInBuffer = sourceWave.dwDataSize;
	DWORD dwInBuffer = cbMaxInBuffer + dwSilenceBytes;
	DWORD dwBlockAlign = pwfxDst->nBlockAlign;

	// Keep the original source size
	DWORD dwOriginalSrcSize = sourceWave.dwDataSize + dwSilenceBytes;

	// Get the number of intermediate formats required for the conversion to be possible
	if(FAILED(CalculateConversionSteps(pwfxSrc, pwfxDst, bCompress)))
	{
		delete[] pwfxSrc;
		return E_FAIL;
	}

	// Cleanup previous compression
	DeleteWaveObjectBuffers(&convertedWaveObj);

	// Get a temporary buffer to hold the source wave
	DMUSP_WAVEOBJECT tempSrcObject;
	ZeroMemory(&tempSrcObject, sizeof(DMUSP_WAVEOBJECT));
	
	if(FAILED(CopyWaveObject(&sourceWave, &tempSrcObject, dwInBuffer)))
	{
		delete[] pwfxSrc;
		return E_FAIL;
	}
	
	// Convert through all intermediate formats
	for(int nIndex = 0; nIndex < m_arrIntermediateFormats.GetSize() - 1; nIndex++)
	{
		// Source format
		WAVEFORMATEX* pSrcWaveFormat = (WAVEFORMATEX*)m_arrIntermediateFormats[nIndex];
		ASSERT(pSrcWaveFormat);

		// Destination format
		WAVEFORMATEX* pDstWaveFormat = (WAVEFORMATEX*)m_arrIntermediateFormats[nIndex + 1];
		ASSERT(pDstWaveFormat);

		// First time convertedWaveObj.pbData is guaranteed to be NULL
		if(nIndex != 0  && convertedWaveObj.pbData != NULL)
		{
			if(pDstWaveFormat->wFormatTag == WMAUDIO_FORMAT_TAG)
			{
				WMAUDIO2WAVEFORMAT* pWMAudio = (WMAUDIO2WAVEFORMAT*) pDstWaveFormat;
				dwBlockAlign = pWMAudio->dwSuperBlockAlign;
			}

			DWORD cbMaxInRequired = MulDiv(dwBlockAlign, pSrcWaveFormat->nAvgBytesPerSec, pDstWaveFormat->nAvgBytesPerSec);
			if(cbMaxInRequired > cbMaxInBuffer)
			{
				// We need a big buffer to hold enough input samples to create an output frame.
				dwInBuffer = cbMaxInRequired * 2;
			}
			else
			{
				dwInBuffer = convertedWaveObj.dwDataSize;
			}

			dwOriginalSrcSize = convertedWaveObj.dwDataSize;
			if(FAILED(CopyWaveObject(&convertedWaveObj, &tempSrcObject, dwInBuffer)))
			{
				return E_FAIL;
			}

			CleanWaveObject(&convertedWaveObj);
		}

		HACMDRIVER hACMDriver = NULL;

#ifdef MSAUDIO
		if(pDstWaveFormat->wFormatTag == MSAUDIO_FORMAT_TAG)
		{
			MMRESULT mmr = acmDriverOpen(&hACMDriver, m_CompressionManager.m_hMSAudioV1BetaDriverID, 0);
			if(mmr == MMSYSERR_NOERROR)
			{
				bCloseBetaDriver = true;
			}
		}
#endif MSAUDIO

		// Find the best driver for PCM format except if we're using MSAudio in which case use the beta V1 driver
		HACMSTREAM has;
		MMRESULT mmr = acmStreamOpen(&has, hACMDriver, pSrcWaveFormat, pDstWaveFormat, NULL, 0, 0, 0);

		if(mmr != MMSYSERR_NOERROR)
		{
			DeleteWaveObjectBuffers(&tempSrcObject);
			delete[] pwfxSrc;
			if(bCloseBetaDriver)
				acmDriverClose(hACMDriver, 0);
			return E_FAIL;	
		}

		// Get the size of the output buffer
		DWORD dwOutPutBytes = 0; 
		mmr = acmStreamSize(has, dwInBuffer, &dwOutPutBytes, ACM_STREAMSIZEF_SOURCE);

		if(mmr != MMSYSERR_NOERROR || dwOutPutBytes == 0)
		{
			acmStreamClose(has, 0);		
			DeleteWaveObjectBuffers(&tempSrcObject);
			delete[] pwfxSrc;
			if(bCloseBetaDriver)
				acmDriverClose(hACMDriver, 0);
			return E_FAIL;	
		}

		ACMSTREAMHEADER ashdr;
		ZeroMemory(&ashdr, sizeof(ashdr));
		ashdr.cbStruct = sizeof(ashdr);

		ashdr.pbSrc = (BYTE*) tempSrcObject.pbData;
		ashdr.cbSrcLength = dwOriginalSrcSize;

		// Add a littel extra room
		//dwOutPutBytes = dwOutPutBytes * 3 / 2;
		ashdr.cbDstLength = dwOutPutBytes;

		if(FAILED(SafeAllocBuffer(&ashdr.pbDst, dwOutPutBytes)))
		{
			acmStreamClose(has, 0);
			DeleteWaveObjectBuffers(&tempSrcObject);
			delete[] pwfxSrc;
			if(bCloseBetaDriver)
			{
				acmDriverClose(hACMDriver, 0);
			}

			return E_OUTOFMEMORY;	
		}

		BYTE* pTempOutBuffer = NULL;
		if(FAILED(SafeAllocBuffer(&pTempOutBuffer, dwOutPutBytes)))
		{
			acmStreamClose(has, 0);
			DeleteWaveObjectBuffers(&tempSrcObject);
			delete[] pwfxSrc;
			delete[] ashdr.pbDst;
			if(bCloseBetaDriver)
			{
				acmDriverClose(hACMDriver, 0);
			}

			return E_OUTOFMEMORY;	
		}

		mmr = acmStreamPrepareHeader(has, &ashdr, 0);	
		if(mmr != MMSYSERR_NOERROR || (ashdr.fdwStatus & ACMSTREAMHEADER_STATUSF_PREPARED) == 0)
		{
			acmStreamClose(has, 0);
			delete[] ashdr.pbDst;
			delete[] pTempOutBuffer;
			DeleteWaveObjectBuffers(&tempSrcObject);
			delete[] pwfxSrc;
			if(bCloseBetaDriver)
				acmDriverClose(hACMDriver, 0);
			return E_FAIL;	
		}

		// Keeps track of the length of the output buffer...
		DWORD dwDstOffset = 0;
		for (DWORD dwSrcConverted = 0; dwSrcConverted < dwOriginalSrcSize;)
		{
			ashdr.cbDstLengthUsed = 0;
			ashdr.cbSrcLengthUsed = 0;

			mmr = acmStreamConvert(has, &ashdr, ACM_STREAMCONVERTF_BLOCKALIGN );
			if(mmr != MMSYSERR_NOERROR)
			{
				break;
			}

			// See how much of the source was actually converted
			dwSrcConverted += ashdr.cbSrcLengthUsed;
			if(dwSrcConverted < tempSrcObject.dwDataSize)
				memmove(ashdr.pbSrc, ashdr.pbSrc + ashdr.cbSrcLengthUsed, tempSrcObject.dwDataSize - dwSrcConverted);
			
			ashdr.cbSrcLength = dwOriginalSrcSize - dwSrcConverted;

			if(ashdr.cbDstLengthUsed == 0)
			{
				break;
			}
			
			// Copy the converted output
			CopyMemory(pTempOutBuffer + dwDstOffset, ashdr.pbDst, ashdr.cbDstLengthUsed);
			dwDstOffset += ashdr.cbDstLengthUsed;
		}

		// CleanUp Pass
		// MSAudio V1 beta codec can not handle the ACM_STREAMCONVERTF_END flag and will crash
		while(1 && pDstWaveFormat->wFormatTag != MSAUDIO_FORMAT_TAG)
		{
			ashdr.cbSrcLengthUsed = 0;
			ashdr.cbDstLengthUsed = 0;

			mmr = acmStreamConvert(has, &ashdr, ACM_STREAMCONVERTF_BLOCKALIGN | ACM_STREAMCONVERTF_END);
			if(mmr != MMSYSERR_NOERROR)
			{
				break;
			}

			// See how much of the source was actually converted
			dwSrcConverted += ashdr.cbSrcLengthUsed;
			if(dwSrcConverted != 0 && dwSrcConverted < tempSrcObject.dwDataSize)
				memmove(ashdr.pbSrc, ashdr.pbSrc + ashdr.cbSrcLengthUsed, tempSrcObject.dwDataSize - dwSrcConverted);
			ashdr.cbSrcLength = dwOriginalSrcSize - dwSrcConverted;

			if(ashdr.cbDstLengthUsed == 0)
			{
				break;
			}
			
			// Copy the converted output
			CopyMemory(pTempOutBuffer + dwDstOffset, ashdr.pbDst, ashdr.cbDstLengthUsed);
			dwDstOffset += ashdr.cbDstLengthUsed;
		}

		// Flush...
		// MSAudio V1 beta codec can not handle the ACM_STREAMCONVERTF_END flag and will crash
		if(ashdr.cbDstLengthUsed == 0 && pDstWaveFormat->wFormatTag != MSAUDIO_FORMAT_TAG /*&& pDstWaveFormat->wFormatTag != WMAUDIO_FORMAT_TAG*/)
		{
			ashdr.cbSrcLengthUsed = 0;
			ashdr.cbDstLengthUsed = 0;

			mmr = acmStreamConvert(has, &ashdr, ACM_STREAMCONVERTF_END);
			if(mmr == MMSYSERR_NOERROR && ashdr.cbDstLengthUsed != 0)
			{
				// Copy the converted output
				CopyMemory(pTempOutBuffer + dwDstOffset, ashdr.pbDst, ashdr.cbDstLengthUsed);
				dwDstOffset += ashdr.cbDstLengthUsed;
			}
		}

		if(mmr != MMSYSERR_NOERROR)
		{
			acmStreamUnprepareHeader(has, &ashdr, 0);		
			acmStreamClose(has, 0);		

			delete [] ashdr.pbDst;
			DeleteWaveObjectBuffers(&tempSrcObject);
			delete[] pwfxSrc;
			delete[] pTempOutBuffer;
			if(bCloseBetaDriver)
			{
				acmDriverClose(hACMDriver, 0);
			}

			return E_FAIL;	
		}

		// Need to reset or acmStreamUnprepareHeader will return with an error
		ashdr.cbSrcLength = dwOriginalSrcSize;
		ashdr.cbDstLength = dwOutPutBytes;

		mmr = acmStreamUnprepareHeader(has, &ashdr, 0);
		if(mmr != MMSYSERR_NOERROR)
		{
			acmStreamClose(has, 0);		
			DeleteWaveObjectBuffers(&tempSrcObject);
			delete [] ashdr.pbDst;
			delete[] pwfxSrc;
			delete[] pTempOutBuffer;
			if(bCloseBetaDriver)
				acmDriverClose(hACMDriver, 0);
			return E_FAIL;	
		}
		
		// Close the ACM stream
		mmr = acmStreamClose(has, 0);
		
		if(mmr != MMSYSERR_NOERROR)
		{
			DeleteWaveObjectBuffers(&tempSrcObject);
			delete [] ashdr.pbDst;
			delete[] pwfxSrc;
			delete[] pTempOutBuffer;
			if(bCloseBetaDriver)
				acmDriverClose(hACMDriver, 0);
			return E_FAIL;	
		}

		CopyMemory(&(convertedWaveObj.WaveFormatEx), pDstWaveFormat, sizeof(WAVEFORMATEX));

		if(pDstWaveFormat->cbSize > 0)
		{
			DWORD dwExtraBytes = pDstWaveFormat->cbSize;
			
			// Pull out the MSAUDIO_KEY
			if(convertedWaveObj.WaveFormatEx.wFormatTag == WMAUDIO_FORMAT_TAG || convertedWaveObj.WaveFormatEx.wFormatTag == MSAUDIO_FORMAT_TAG)
			{
				dwExtraBytes = pDstWaveFormat->cbSize - sizeof(MSAUDIO_KEY);
				convertedWaveObj.WaveFormatEx.cbSize = (USHORT)dwExtraBytes;
			}

			convertedWaveObj.pbExtractWaveFormatData = new BYTE[dwExtraBytes];
			
			if(convertedWaveObj.pbExtractWaveFormatData == NULL)
			{
				DeleteWaveObjectBuffers(&tempSrcObject);
				delete[] pwfxSrc;
				if(bCloseBetaDriver)
					acmDriverClose(hACMDriver, 0);
				return E_FAIL;
			}
			else
			{
				ZeroMemory(convertedWaveObj.pbExtractWaveFormatData, dwExtraBytes);
				CopyMemory(convertedWaveObj.pbExtractWaveFormatData, (((BYTE*)pDstWaveFormat) + sizeof(WAVEFORMATEX)), dwExtraBytes);
			}
		}

		convertedWaveObj.pbData = new BYTE[dwDstOffset];

		// Out of memory??
		if(convertedWaveObj.pbData == NULL)
		{
			delete [] ashdr.pbDst;
			delete[] pTempOutBuffer;
			DeleteWaveObjectBuffers(&tempSrcObject);
			if(bCloseBetaDriver)
				acmDriverClose(hACMDriver, 0);

			return E_FAIL;
		}
		
		ZeroMemory(convertedWaveObj.pbData, dwDstOffset);
		CopyMemory(convertedWaveObj.pbData, pTempOutBuffer, dwDstOffset);
		convertedWaveObj.dwDataSize = dwDstOffset;
		
		delete [] ashdr.pbDst;
		delete[] pTempOutBuffer;
		DeleteWaveObjectBuffers(&tempSrcObject);
		if(bCloseBetaDriver)
			acmDriverClose(hACMDriver, 0);
	}
 
	// If we're decompressing MSAudio file then compute how many
	// silent samples in the beginning we need to throw away....
	if(!bCompress && pwfxSrc->wFormatTag == WMAUDIO_FORMAT_TAG)
	{
		// V1
		int nVersion = 1;
		if(pwfxSrc->wFormatTag == WMAUDIO_FORMAT_TAG)
			nVersion = 2;	// V2
		
		int	nSamplesPerSec = pwfxSrc->nSamplesPerSec;
		DWORD dwBitPerSec = pwfxSrc->nSamplesPerSec * pwfxSrc->wBitsPerSample;
		int nNumChannels = pwfxSrc->nChannels;
		
		int nActualSamples = msaudioGetSamplePerFrame(nSamplesPerSec, dwBitPerSec, nNumChannels, nVersion);
		if(pwfxSrc->wBitsPerSample == 16)
			nActualSamples *= 2;

		if((int)convertedWaveObj.dwDataSize > nActualSamples)
		{
			DWORD dwActualDataSize = convertedWaveObj.dwDataSize - nActualSamples;
			BYTE* pTempBuffer = new BYTE[dwActualDataSize];
			if(pTempBuffer)
			{
				ZeroMemory(pTempBuffer, dwActualDataSize);
				CopyMemory(pTempBuffer, convertedWaveObj.pbData + nActualSamples, dwActualDataSize);
				delete[] convertedWaveObj.pbData;
				convertedWaveObj.pbData = pTempBuffer;
				convertedWaveObj.dwDataSize = dwActualDataSize;
			}
		}
	}

	// Reset the waveformat values we changed if we're decompressing and the format is PCM
	if(bCompress == false && convertedWaveObj.WaveFormatEx.wFormatTag == 1)
	{
		convertedWaveObj.WaveFormatEx.nSamplesPerSec = pwfxDst->nSamplesPerSec;
		convertedWaveObj.WaveFormatEx.nAvgBytesPerSec = convertedWaveObj.WaveFormatEx.nSamplesPerSec * convertedWaveObj.WaveFormatEx.nBlockAlign;
	}

	delete[] pwfxSrc;
	return S_OK;
}


// If there's a difference between the decompressed and the original 
// append silence equivalent to that difference and compress again
HRESULT CWave::FindSampleLoss(const WAVEFORMATEX* pwfxDest, DWORD& dwDifference)
{
	HRESULT hr = E_FAIL;
	ASSERT(pwfxDest);
	if(pwfxDest == NULL)
		return E_FAIL;

	// What is the original wave's length?
	DWORD dwOldWaveLength = m_OriginalWavObj.dwDataSize;
	if(m_OriginalWavObj.WaveFormatEx.wBitsPerSample != 8)
		dwOldWaveLength = m_OriginalWavObj.dwDataSize >> 1;
	
	DMUSP_WAVEOBJECT tempCompressedWaveObject;
	DMUSP_WAVEOBJECT tempDeCompressedWaveObject;

	ZeroMemory(&(tempCompressedWaveObject), sizeof(DMUSP_WAVEOBJECT));
	ZeroMemory(&(tempDeCompressedWaveObject), sizeof(DMUSP_WAVEOBJECT));

	// Compress the wave and if it succeeds...
	if(SUCCEEDED(ConvertWave(m_OriginalWavObj, pwfxDest, tempCompressedWaveObject, true)))
	{
		// ...decompress the wave
		if(FAILED(ConvertWave(tempCompressedWaveObject, &(m_rWaveformat), tempDeCompressedWaveObject, false)))
		{
			CleanWaveObject(&tempCompressedWaveObject);
			return E_FAIL;
		}
		// Then find out the difference in wavelengths
		// and append the silence...
		DWORD dwNewWaveLength = tempDeCompressedWaveObject.dwDataSize;
		if(tempDeCompressedWaveObject.WaveFormatEx.wBitsPerSample != 8)
			dwNewWaveLength = tempDeCompressedWaveObject.dwDataSize >> 1;

		dwDifference = abs(dwOldWaveLength - dwNewWaveLength);

		CleanWaveObject(&tempCompressedWaveObject);
		CleanWaveObject(&tempDeCompressedWaveObject);

		hr = S_OK;
	}

	return hr;
}


// Copied from MSAUDIO.CPP
int CWave::msaudioGetSamplePerFrame(int cSamplePerSec, DWORD dwBitPerSec, int cNumChannels, int nVersion)
{
	// return NEW samples coming into a frame; actual samples in a frame
	// should be * 2 due to 50% overlapping window
	int cSamplePerFrame;
    if (cSamplePerSec <= 8000) 
	{
        cSamplePerFrame = 512;
    }
    else if (cSamplePerSec <= 11025)
        cSamplePerFrame = 512;
    else if (cSamplePerSec <= 16000)
        cSamplePerFrame = 512;
    else if (cSamplePerSec <= 22050)
        cSamplePerFrame = 1024;
    else if (cSamplePerSec <= 32000)
	{
		if(nVersion == 1)
			cSamplePerFrame = 1024;
		else
			cSamplePerFrame = 2048;
	}
    else if (cSamplePerSec <= 44100)
        cSamplePerFrame = 2048;
    else if (cSamplePerSec <= 48000)
        cSamplePerFrame = 2048;
    else
        return 0;
    WORD wBytesPerFrame = WORD(((cSamplePerFrame*dwBitPerSec + cSamplePerSec/2)/cSamplePerSec + 7)/8);
    if (wBytesPerFrame <= 1) 
	{   // silence mode
        while (wBytesPerFrame == 0) 
		{
            cSamplePerFrame *= 2;           // save more bits; quartz can't take too big a value
            wBytesPerFrame = WORD(((cSamplePerFrame*dwBitPerSec + cSamplePerSec/2)/cSamplePerSec + 7)/8);
        }
    }
    return cSamplePerFrame;
}

int CWave::GetTimePlayed()
{
	return m_nTimePlayed;
}

CWaveDataManager* CWave::GetDataManager()
{
	return m_pDataManager;
}

HRESULT CWave::GetData(DWORD dwStartSample, DWORD dwLength, BYTE** ppbData, DWORD& dwBytesRead, bool bGetUncompressedOnly)
{
	dwBytesRead = 0;

	ASSERT(m_pDataManager);
	if(m_pDataManager == NULL)
	{
		return E_FAIL;
	}

	if(dwLength == 0)
	{
		dwBytesRead = 0;
		return S_OK;
	}

	int nSampleSize = m_rWaveformat.wBitsPerSample / 8;
	int nChannels = m_rWaveformat.nChannels;
   
    if(bGetUncompressedOnly)
    {
        nSampleSize = m_OriginalWavObj.WaveFormatEx.wBitsPerSample / 8;
        nChannels = m_OriginalWavObj.WaveFormatEx.nChannels;
    }

    DWORD dwBufferSize = dwLength * nSampleSize * nChannels;

	if(FAILED(SafeAllocBuffer(ppbData, dwBufferSize)))
	{
		return E_FAIL;
	}

	EnterCriticalSection(&m_pWaveNode->m_CriticalSection);
	if(FAILED(m_pDataManager->GetData(dwStartSample, dwLength, *ppbData, dwBytesRead, bGetUncompressedOnly)))
	{
		LeaveCriticalSection(&m_pWaveNode->m_CriticalSection);
		delete[] *ppbData;
		dwBytesRead = 0;
		return E_FAIL;
	}

	LeaveCriticalSection(&m_pWaveNode->m_CriticalSection);
	return S_OK;
}


HRESULT CWave::GetAllUncompressedData(BYTE** ppbData, DWORD* pdwSize)
{
    ASSERT(pdwSize);
    if(pdwSize == NULL)
    {
        return E_POINTER;
    }

	ASSERT(m_pDataManager);
	if(m_pDataManager == NULL)
	{
		return E_FAIL;
	}

	EnterCriticalSection(&m_pWaveNode->m_CriticalSection);
	if(FAILED(m_pDataManager->GetAllUncompressedData(ppbData, pdwSize)))
	{
		LeaveCriticalSection(&m_pWaveNode->m_CriticalSection);
		*pdwSize = 0;
		return E_FAIL;
	}

	LeaveCriticalSection(&m_pWaveNode->m_CriticalSection);
	return S_OK;

}

HRESULT CWave::WriteDataToClipboard(IStream* pIStream, DWORD dwStartSample, DWORD dwClipLength)
{
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

	BYTE* pbData = NULL;
	DWORD dwDataSize = 0;
	if(FAILED(GetData(dwStartSample, dwClipLength, &pbData, dwDataSize)))
	{
		return E_OUTOFMEMORY;
	}

	if(FAILED(pIStream->Write((LPSTR) pbData, dwDataSize, &cb)))	
	{
		delete[] pbData;
		return E_FAIL;
	}

	delete[] pbData;

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


HRESULT CWave::ReadDataFromClipbaord(IStream* pIStream, WAVEFORMATEX& wfxClipFormat, BYTE** ppbData, DWORD& dwDataSize)
{
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_POINTER;
	}

	IDMUSProdRIFFStream* pIRiffStream = NULL;
	if (FAILED(AllocRIFFStream(pIStream, &pIRiffStream)))
	{
		return E_FAIL;
	}


	// Seek to beginning of stream
	LARGE_INTEGER liTemp;
	liTemp.QuadPart = 0;
	pIStream->Seek(liTemp, STREAM_SEEK_SET, NULL);

	ZeroMemory(&wfxClipFormat, sizeof(WAVEFORMATEX));

	dwDataSize = 0;

	MMCKINFO ckMain;
	MMCKINFO ck;
	ckMain.fccType = mmioFOURCC('W','A','V','E') ;
	if(pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
	{
		bool bReadFormat = false;

		DWORD cb = 0;
		ck.ckid = 0;
		ck.fccType = 0;
		while( pIRiffStream->Descend(&ck, &ckMain, 0) == 0)
		{
			switch (ck.ckid) 
			{
				case mmioFOURCC('f','m','t',' '):
				{
					DWORD dwReadSize = 0;
					bReadFormat = TRUE;
					if(ck.cksize < sizeof(WAVEFORMATEX))
					{
						dwReadSize = ck.cksize;
					}
					else
					{
						dwReadSize = sizeof(WAVEFORMATEX);
					}

					HRESULT hr = pIStream->Read(&wfxClipFormat, dwReadSize, &cb);
					if(FAILED(hr) || cb != dwReadSize)
					{
						pIRiffStream->Release();
						return E_FAIL;
					}
					
					if(wfxClipFormat.wFormatTag != WAVE_FORMAT_PCM)
					{
						pIRiffStream->Release();
						return E_FAIL;
					}

					if(wfxClipFormat.nSamplesPerSec != m_rWaveformat.nSamplesPerSec)
					{
						if (IDCANCEL == AfxMessageBox(IDS_WAVE_SAMPLE_RATE, MB_ICONEXCLAMATION | MB_OKCANCEL))
						{
							pIRiffStream->Release();
							return E_FAIL;
						}
					}
					
					break;            
				}
				
				case mmioFOURCC('d','a','t','a'):
				{
					if (bReadFormat)
					{
						if(FAILED(SafeAllocBuffer(ppbData, ck.cksize)))
						{
							return E_OUTOFMEMORY;
						}
						
						if(*ppbData != NULL)
						{
							HRESULT hr = pIStream->Read(*ppbData, ck.cksize, &cb );
							if(FAILED(hr) || cb != ck.cksize)
							{
								delete[] *ppbData;
								pIRiffStream->Release();
								return E_FAIL;
							}

							dwDataSize = ck.cksize;

							// Use files less than 1M
							/*if(dwDataSize >= (1 << 20))    
							{
								AfxMessageBox(IDS_WAVE_OVER_ONE_MEG, MB_ICONEXCLAMATION);
								delete[] *ppbData;
								pIRiffStream->Release();
								return E_FAIL;
							}*/
						}
						else
						{
							// Out Of Memory!!
							pIRiffStream->Release();
							return E_OUTOFMEMORY;
						}
					}
					else 
					{
						// Format must come first in a wave RIFF file
						pIRiffStream->Release();
						return E_FAIL;
					}
					
					break;
				}

				default:
					break;
			}

			if (ck.cksize == 0)
				break;

			pIRiffStream->Ascend( &ck, 0 );
			ck.ckid = 0;
			ck.fccType = 0;
		}
	}
	else
	{
		return E_FAIL;
	}
	
	pIRiffStream->Release();
	return S_OK;
}

HRESULT CWave::Fade(bool bFadeDirection, DWORD dwStart, DWORD dwEnd)
{
	int nSelectedSamples = abs(dwEnd - dwStart);
	int nBytesPerSample = 1;
	if(m_rWaveformat.wBitsPerSample > 8)
	{
		nBytesPerSample = m_rWaveformat.wBitsPerSample / 8;
	}

	int nChannels = m_rWaveformat.nChannels;

	BYTE* pbData = NULL;
	DWORD cb = 0;
	if(FAILED(GetData(dwStart, nSelectedSamples, &pbData, cb)))
	{
		return E_FAIL;
	}

	// If this is a stereo wave then the actual number of samples is doubled
	// But we still want to calculate the fade factor on the passed length..
	int nSelectionLength = nSelectedSamples * nChannels;

	for(int nSampleCount = 0; nSampleCount <= nSelectionLength; nSampleCount++)
	{
		if(nBytesPerSample == 2)
		{
			short* pnData = (short*)pbData;
			int nSampleValue = pnData[nSampleCount];
			
			if(bFadeDirection)
			{
				nSampleValue = (int)((nSampleValue * ((float)(nSampleCount) / nSelectionLength)) + 0.5);
			}
			else
			{
				nSampleValue = (int)((nSampleValue * ((float)(nSelectionLength - nSampleCount) / nSelectionLength)) + 0.5);
			}
			
			pnData[nSampleCount] = short(nSampleValue);
		}
		else if(nBytesPerSample == 1)
		{
			BYTE byteSampleValue = *(pbData + nSampleCount);
			__int8 nSignedValue = byteSampleValue - 128;
			
			if(bFadeDirection)
			{
				nSignedValue = (__int8)((nSignedValue * ((float)(nSampleCount) / nSelectedSamples)) + 0.5);
			}
			else
			{
				nSignedValue = (__int8)((nSignedValue * ((float)(nSelectionLength - nSampleCount) / nSelectedSamples)) + 0.5);
			}
			
			*(pbData + nSampleCount) = nSignedValue + 128;
		}
	}

	// Remove the old data and replace with the faded out
	if(FAILED(m_pDataManager->RemoveData(dwStart, nSelectedSamples)))
	{
		delete[] pbData;
		return E_FAIL;
	}

	if(FAILED(m_pDataManager->InsertData(dwStart, pbData, nSelectedSamples)))
	{
		delete[] pbData;
		return E_FAIL;
	}

	return S_OK;
}

inline double resize_h(double t)
{
	double o;
	t=fabs(t);
	if (t<1.0)
	{
		o=7.0*t*t*t-12.0*t*t+16.0/3.0;
	}
	else if (t<2.0)
	{
		o=-7.0*t*t*t/3.0+12.0*t*t-20.0*t+32.0/3.0;
	}
	else o=0.0;
	return o;
}

// Minify, or downsample
void Downsample16Bit(const short *pwInput, // Source
               short *pwOutput, // Destination
               const int nInputSampleLength,
			   const int nInputStart,
               const int nOutputSampleLength,
			   const DWORD dwNumChannels,
			   const double sx )
{
	for (int x=0; x<nOutputSampleLength; x++)
	{
		for (DWORD dwChannel = 0; dwChannel < dwNumChannels; dwChannel++)
		{
			double rs=0.0;
            double ttSum=0.0;

			for (int t=(int)floor(-2.0+x/sx); t<=ceil(2.0+x/sx); t++)
			{
    			double tt = resize_h(x-t*sx);
                ttSum+=tt;

				int nIndex = nInputStart + t;
				if( nIndex >= 0 && nIndex < nInputSampleLength )
				{
					rs+=double(pwInput[nIndex * dwNumChannels + dwChannel])*tt;
				}
			}
			rs /= ttSum;
			if (rs>32767.0)
				rs=32767.0;
			if (rs<-32768.0)
				rs=-32768.0;
			pwOutput[x * dwNumChannels + dwChannel]= short(rs);
		}
	}
}

// Magnify, or upsample
void Upsample16Bit(const short *pwInput, // Source
               short *pwOutput, // Destination
               const int nInputSampleLength,
			   const int nInputStart,
               const int nOutputSampleLength,
			   const DWORD dwNumChannels,
			   const double sx )
{
	for (int x=0; x<nOutputSampleLength; x++)
	{
		for (DWORD dwChannel = 0; dwChannel < dwNumChannels; dwChannel++)
		{
		    double rs=0.0;
		    for (int t=(int)floor(-2.0+x/sx); t<=ceil(2.0+x/sx); t++)
		    {
				int nIndex = nInputStart + t;
			    if ((nIndex<nInputSampleLength)&&(nIndex>=0))
			    {
    				double tt=resize_h(x/sx-t);
					rs+=double(pwInput[nIndex * dwNumChannels + dwChannel])*tt;
			    }
		    }
            rs /= 6.0;
		    if (rs>32767.0)
			    rs=32767.0;
		    if (rs<-32768.0)
			    rs=-32768.0;
			pwOutput[x * dwNumChannels + dwChannel]=short(rs);
        }
	}
}

// Minify, or downsample
void Downsample8Bit(const BYTE *pbInput, // Source
               BYTE *pbOutput, // Destination
               const int nInputSampleLength,
			   const int nInputStart,
               const int nOutputSampleLength,
			   const DWORD dwNumChannels,
			   const double sx )
{
	for (int x=0; x<nOutputSampleLength; x++)
	{
		for (DWORD dwChannel = 0; dwChannel < dwNumChannels; dwChannel++)
		{
			double rs=0.0;
            double ttSum=0.0;

			for (int t=(int)floor(-2.0+x/sx); t<=ceil(2.0+x/sx); t++)
			{
    			double tt = resize_h(x-t*sx);
                ttSum+=tt;
				int nIndex = nInputStart + t;
				if( nIndex >= 0 && nIndex < nInputSampleLength )
				{
					rs+=double(int(pbInput[nIndex * dwNumChannels + dwChannel]) - 128)*tt;
				}
			}
			rs /= ttSum;
			if (rs>127.0)
				rs=127.0;
			if (rs<-128.0)
				rs=-128.0;
			pbOutput[x * dwNumChannels + dwChannel]= int(rs) + 128;
		}
	}
}

// Magnify, or upsample
void Upsample8Bit(const BYTE *pbInput, // Source
               BYTE *pbOutput, // Destination
               const int nInputSampleLength,
			   const int nInputStart,
               const int nOutputSampleLength,
			   const DWORD dwNumChannels,
			   const double sx )
{
	for (int x=0; x<nOutputSampleLength; x++)
	{
		for (DWORD dwChannel = 0; dwChannel < dwNumChannels; dwChannel++)
		{
		    double rs=0.0;
		    for (int t=(int)floor(-2.0+x/sx); t<=ceil(2.0+x/sx); t++)
		    {
				int nIndex = nInputStart + t;
			    if ((nIndex<nInputSampleLength)&&(nIndex>=0))
			    {
    				double tt=resize_h(x/sx-t);
					rs+=double(int(pbInput[nIndex * dwNumChannels + dwChannel]) - 128)*tt;
			    }
		    }
            rs /= 6.0;
		    if (rs>127.0)
			    rs=127.0;
		    if (rs<-128.0)
			    rs=-128.0;
			pbOutput[x * dwNumChannels + dwChannel]=int(rs) + 128;
        }
	}
}

HRESULT CWave::Resample(DWORD dwNewSampleRate)
{
	if( m_rWaveformat.nSamplesPerSec == dwNewSampleRate )
	{
		return S_OK;
	}

	UINT nBytesPerSample = m_rWaveformat.wBitsPerSample / 8;
	UINT nSampleSize = nBytesPerSample * m_rWaveformat.nChannels;
		
	// The data range could be huge so we need to do this in chunks of 8K
	// Unfortunately, using chunks we end up moving everything 3 samples earlier.
	// So, for now we resample the entire wave at once.
	const DWORD dwChunkSize = m_dwWaveLength;	
	DWORD dwChunkStart = 0;
	DWORD dwSamplesToWrite = m_dwWaveLength;
	DWORD dwOrigStart = 0;
	const int nHalfBasis = 3;

	while( dwSamplesToWrite > 0)
	{
		// Read in a chunk of data
		BYTE* pbData = NULL;
		DWORD cbRead = 0;
		const DWORD dwSamplesRead = min( dwChunkSize, dwSamplesToWrite);
		if( FAILED(GetData(dwChunkStart, dwSamplesRead, &pbData, cbRead))
		||	(cbRead / nSampleSize) != dwSamplesRead )
		{
			return E_FAIL;
		}

		// Compute the samples in this chunk to start and end resampling at
		DWORD dwSampleToStartAt, dwSampleToEndAt;

		// Flag whether or not we're in the first or last chunk
		// If there is only one chunk, then these should stay false
		bool fEndChunk = false;
		bool fStartChunk = false;

		// If we're going to do this again
		if( dwSamplesRead < dwSamplesToWrite )
		{
			// If this is the first chunk
			if( dwChunkStart == 0 )
			{
				fStartChunk = true;
				dwSampleToStartAt = 0;
				dwSampleToEndAt = dwSamplesRead - nHalfBasis;
			}
			else
			{
				dwSampleToStartAt = nHalfBasis;
				dwSampleToEndAt = dwSamplesRead - nHalfBasis * 2;
			}
		}
		// If this is the entire waveform
		else if( dwSamplesRead == m_dwWaveLength )
		{
			dwSampleToStartAt = 0;
			dwSampleToEndAt = dwSamplesRead;
		}
		// Else this is the end chunk
		else
		{
			fEndChunk = true;
			dwSampleToStartAt = nHalfBasis;
			dwSampleToEndAt = dwSamplesRead;
		}

		// Compute the number of samples we're resizing to
		const DWORD dwResampledSize = DWORD(RESAMPLE_SIZE_CHANGE(dwNewSampleRate, m_rWaveformat.nSamplesPerSec, dwOrigStart + dwSampleToEndAt) - RESAMPLE_SIZE_CHANGE(dwNewSampleRate, m_rWaveformat.nSamplesPerSec, dwOrigStart + dwSampleToStartAt));

		// Allocate the buffer to resample into
		BYTE *pbResampledData = new BYTE[dwResampledSize * nSampleSize];
		if( !pbResampledData )
		{
			delete[] pbData;
			return E_FAIL;
		}		

		/*
		// Simple pitch shift without speed up or slowing down..
		{
			const DWORD dwSizeToCopy = min(dwResampledSize, dwSampleToEndAt - dwSampleToStartAt);
			DWORD dwLeft = dwResampledSize;
			BYTE *pbDest = pbResampledData;
			while( dwLeft > 0 )
			{
				memcpy( pbDest, pbData, dwSizeToCopy * nSampleSize );
				dwLeft -= dwSizeToCopy;
				pbDest += dwSizeToCopy * nSampleSize;
			}
		}
		*/

		// Actual resampling
		const BYTE *pbStart = pbData + dwSampleToStartAt * m_rWaveformat.nChannels * nBytesPerSample;
		if( nBytesPerSample == 1 )
		{
			if( dwNewSampleRate < m_rWaveformat.nSamplesPerSec )
			{
				Downsample8Bit( pbStart, pbResampledData, dwSamplesRead, dwSampleToStartAt, dwResampledSize, m_rWaveformat.nChannels, double(dwNewSampleRate) / double(m_rWaveformat.nSamplesPerSec) );
			}
			else
			{
				Upsample8Bit( pbStart, pbResampledData, dwSamplesRead, dwSampleToStartAt, dwResampledSize, m_rWaveformat.nChannels, double(dwNewSampleRate) / double(m_rWaveformat.nSamplesPerSec) );
			}
		}
		else if( nBytesPerSample == 2 )
		{
			if( dwNewSampleRate < m_rWaveformat.nSamplesPerSec )
			{
				Downsample16Bit( (short *)pbStart, (short *)pbResampledData, dwSamplesRead, dwSampleToStartAt, dwResampledSize, m_rWaveformat.nChannels, double(dwNewSampleRate) / double(m_rWaveformat.nSamplesPerSec) );
			}
			else
			{
				Upsample16Bit( (short *)pbStart, (short *)pbResampledData, dwSamplesRead, dwSampleToStartAt, dwResampledSize, m_rWaveformat.nChannels, double(dwNewSampleRate) / double(m_rWaveformat.nSamplesPerSec) );
			}
		}

		// Delete the original data, since we no longer need it
		delete[] pbData;

		// If this is the end chunk, we need to remove nHalfBasis more samples
		// If this is the start chunk, we need to remove nHalfBasis fewer samples
		DWORD dwRemoveSize = (fEndChunk ? nHalfBasis : 0) + (fStartChunk ? -nHalfBasis : 0) + dwSampleToEndAt - dwSampleToStartAt;

		// Remove the old data and replace with the faded out
		if(FAILED(m_pDataManager->RemoveData(dwChunkStart, dwRemoveSize)))
		{
			delete[] pbResampledData;
			return E_FAIL;
		}

		// Update the wave size, since we've removed a chunk of data
		m_OriginalWavObj.dwDataSize -= dwRemoveSize * nSampleSize;
		m_dwDataSize = m_OriginalWavObj.dwDataSize;
		m_dwWaveLength -= dwRemoveSize;

		// Insert the new (resampled) data
		if(FAILED(InsertWaveData(pbResampledData, m_rWaveformat, dwResampledSize, dwChunkStart)))
		{
			delete[] pbResampledData;
			return E_FAIL;
		}

		// Update the wave size, since we've inserted the resampled data
		dwSamplesToWrite -= dwRemoveSize;
		dwChunkStart += dwResampledSize;
		dwOrigStart += dwRemoveSize;
	}

	// Update the loop points
	m_rRLOOP.dwStart = DWORD(RESAMPLE_SIZE_CHANGE(dwNewSampleRate, m_rWaveformat.nSamplesPerSec, m_rRLOOP.dwStart));
	m_rRLOOP.dwEnd = DWORD(RESAMPLE_SIZE_CHANGE(dwNewSampleRate, m_rWaveformat.nSamplesPerSec, m_rRLOOP.dwEnd));
	SetLoop( m_rRLOOP.dwStart, m_rRLOOP.dwEnd );

	// Update the selection
	CWaveCtrl* pWaveEditor = GetWaveEditor();
	if (pWaveEditor != NULL)
	{
		CDLSComponent* pComponent = NULL;
		CWaveNode* pWaveNode = GetNode();
		if(pWaveNode)
		{
			pComponent = pWaveNode->GetComponent();
		}

		int nSelStart, nSelEnd;
		pWaveEditor->GetSelection(nSelStart, nSelEnd);
		nSelStart = int(RESAMPLE_SIZE_CHANGE(dwNewSampleRate, m_rWaveformat.nSamplesPerSec, nSelStart));;
		nSelEnd = int(RESAMPLE_SIZE_CHANGE(dwNewSampleRate, m_rWaveformat.nSamplesPerSec, nSelEnd));;
		pWaveEditor->SetSelection(nSelStart, nSelEnd, (pComponent != NULL) && pComponent->IsSnapToZero());
	}

	// Update the sample rate
	m_rWaveformat.nSamplesPerSec = dwNewSampleRate;
    m_rWaveformat.nAvgBytesPerSec = dwNewSampleRate * nSampleSize;
	m_OriginalWavObj.WaveFormatEx.nSamplesPerSec = m_rWaveformat.nSamplesPerSec;
	m_OriginalWavObj.WaveFormatEx.nAvgBytesPerSec = m_rWaveformat.nAvgBytesPerSec;

	return S_OK;
}


HRESULT CWave::PrepareForSave(CString sNewFileName)	
{
	ASSERT(m_pDataManager);
	if(m_pDataManager == NULL)
	{
		return E_POINTER;
	}
	
	// We need to rename ourselves if we're NOT in a collection
	if(m_pCollection == NULL)
	{
		if(FAILED(m_pDataManager->RenameSource(sNewFileName)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}



HRESULT CWave::CleanupAfterSave(CString sFileName)
{
	ASSERT(m_pDataManager);
	if(m_pDataManager == NULL)
	{
		return E_FAIL;
	}
	
	ASSERT(m_pWaveNode);
	if(m_pWaveNode == NULL)
	{
		return E_FAIL;
	}

	if(FAILED(m_pDataManager->Initialize(sFileName, m_dwHeaderOffset, m_dwDataOffset, true)))
	{
		return E_FAIL;
	}

	// Set the name for the wave node 
	m_pWaveNode->SetFileName(sFileName);

    m_pWaveNode->SetSourceInTempFile(FALSE);

	return S_OK;
}


CString CWave::GetUndoMenuText(bool bRedo)
{
    ASSERT(m_pDataManager);
    if(m_pDataManager == NULL)
    {
        return "";
    }

    return m_pDataManager->GetUndoMenuText(bRedo);
}


HRESULT CWave::Undo()
{
    ASSERT(m_pDataManager);
    if(m_pDataManager == NULL)
    {
        return E_FAIL;
    }

    return m_pDataManager->Undo();
}


HRESULT CWave::Redo()
{
    ASSERT(m_pDataManager);
    if(m_pDataManager == NULL)
    {
        return E_FAIL;
    }

    return m_pDataManager->Redo();
}

HRESULT CWave::PopUndoState()
{
    ASSERT(m_pDataManager);
    if(m_pDataManager == NULL)
    {
        return E_FAIL;
    }

    return m_pDataManager->PopUndoState();
}

void CWave::UpdateOnUndo(HEADER_INFO headerInfo)
{
	m_dwWaveLength = headerInfo.m_dwWaveLength;
	m_dwDataSize = m_dwWaveLength * (m_rWaveformat.wBitsPerSample / 8) * m_rWaveformat.nChannels;
	if(m_bCompressed)
	{
		m_DecompressedWavObj.dwDataSize = m_dwDataSize;
	}
	else
	{
		m_OriginalWavObj.dwDataSize = m_dwDataSize;
	}
    
    m_bCompressed = headerInfo.m_bCompressed;

	SetWSMPL(headerInfo.m_rWSMP);
	SetWLOOP(headerInfo.m_rWLOOP);
	SetRSMPL(headerInfo.m_rRSMP);
	SetRLOOP(headerInfo.m_rRLOOP);


	if(m_rWSMP.cSampleLoops > 0)
	{
		m_bPlayLooped = true;
	}
	else
	{
		m_bPlayLooped = false;
	}

	m_bStreaming = headerInfo.m_bStreaming;
	m_bNoPreroll = headerInfo.m_bNoPreroll;
	m_dwReadAheadTime = headerInfo.m_dwReadAheadTime;
	m_dwDecompressedStart = headerInfo.m_dwDecompressedStart;

	// Update the sample rate
	m_rWaveformat.nSamplesPerSec = headerInfo.m_dwSamplerate;
    m_rWaveformat.nAvgBytesPerSec = headerInfo.m_dwSamplerate * (m_rWaveformat.wBitsPerSample / 8) * m_rWaveformat.nChannels;
	m_OriginalWavObj.WaveFormatEx.nSamplesPerSec = m_rWaveformat.nSamplesPerSec;
	m_OriginalWavObj.WaveFormatEx.nAvgBytesPerSec = m_rWaveformat.nAvgBytesPerSec;
}

void CWave::NotifyWaveChange(bool bSetDirtyFlag)
{
	ASSERT(m_pWaveNode);
	if(m_pWaveNode == NULL)
	{
		return;
	}

	CDLSComponent* pComponent = m_pWaveNode->GetComponent();
	ASSERT(pComponent);
	if(pComponent == NULL)
	{
		return;
	}

	IDMUSProdFramework* pIFramework = pComponent->m_pIFramework;
	ASSERT(pIFramework);
	if(pIFramework == NULL)
	{
		return;
	}

	// Create a new version GUID
	m_pWaveNode->CreateNewVersionGUID();

	IDMUSProdNode* pIDocRootNode = NULL;
	if(SUCCEEDED(m_pWaveNode->GetDocRootNode(&pIDocRootNode)))
	{
		pIFramework->NotifyNodes(pIDocRootNode, WAVENODE_DataChange, NULL);
		pIDocRootNode->Release();
	}

	if (bSetDirtyFlag)
		SetDirtyFlag();
}


IStream* CWave::GetHeaderMemStream()
{
	return m_pIMemHeaderStream;
}

HRESULT	 CWave::UpdateHeaderStream()
{
	ASSERT(m_pWaveNode);
	if(m_pWaveNode == NULL)
	{
		return E_UNEXPECTED;
	}

	if(m_pIMemHeaderStream)
	{
		m_pIMemHeaderStream->Release();
		m_pIMemHeaderStream = NULL;
	}
	
	CDLSComponent* pComponent = m_pWaveNode->GetComponent();
	ASSERT(pComponent);
	IDMUSProdFramework* pIFramework = pComponent->m_pIFramework;
	ASSERT(pIFramework);
	if(FAILED(pIFramework->AllocMemoryStream(FT_RUNTIME, GUID_CurrentVersion, &m_pIMemHeaderStream)))
	{
		return E_FAIL;
	}

	HRESULT hr = WriteRIFFHeaderToStream(m_pIMemHeaderStream, &m_dwRIFFHeaderSize);
	StreamSeek(m_pIMemHeaderStream, 0, STREAM_SEEK_SET);

	//m_pWaveNode->ReloadDirectSoundWave();

	return hr;
}


DWORD CWave::GetRIFFHeaderSize()
{
	return m_dwRIFFHeaderSize;
}


void CWave::SetHeaderOffset(DWORD dwOffset)
{
	m_dwHeaderOffset = dwOffset;
}


void CWave::SetDataOffset(DWORD dwOffset)
{
	m_dwDataOffset = dwOffset;
}	


void CWave::ValidateDummyPatch()
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
	CDLSComponent* pComponent = m_pWaveNode->GetComponent();
	ASSERT(pComponent);
	if(pComponent == NULL)
	{
		return;
	}

	if(m_pDummyInstrument == NULL)
	{
		return;
	}

	HRESULT hr = pComponent->IsValidDummyPatch(m_pDummyInstrument, m_pDummyInstrument->m_rInstHeader.Locale.ulBank, m_pDummyInstrument->m_rInstHeader.Locale.ulInstrument);
	if (!SUCCEEDED(hr))
	{
		BYTE DummyMSB = 0;
		BYTE DummyLSB = 0;
		BYTE DummyPatch = 0;

		hr = pComponent->GetNextValidDummyPatch(m_pDummyInstrument, &DummyMSB, &DummyLSB, &DummyPatch);
		if (SUCCEEDED(hr))
		{
			m_pDummyInstrument->m_rInstHeader.Locale.ulBank = MAKE_BANK(0, DummyMSB, DummyLSB);
			m_pDummyInstrument->m_rInstHeader.Locale.ulInstrument = (ULONG) DummyPatch;
			m_pDummyInstrument->UpdateInstrument();
		}
		else
		{//To Do: warn user 
		}
	}
}

CInstrument* CWave::GetDummyInstrument()
{
	return m_pDummyInstrument;
}


void CWave::TurnOffMidiNotes()
{

	if( m_bIsPlaying )
	{
		HRESULT hr = E_FAIL;
		ASSERT(m_pWaveNode);
		CDLSComponent* pComponent = m_pWaveNode->GetComponent();
		ASSERT(pComponent);

		hr = pComponent->PlayMIDIEvent(MIDI_NOTEOFF,
    									(BYTE)m_sUnityNoteUsedToTriggerPlay,
										DEFAULT_VELOCITY,
    									0,
										m_pDummyInstrument->IsDrum());	
       if (SUCCEEDED(hr))
       {
			pComponent->m_pIConductor->SetBtnStates(m_pWaveNode, BS_PLAY_ENABLED | BS_NO_AUTO_UPDATE);
            m_bIsPlaying = FALSE;
       }
	}
}

HRESULT CWave::Play(BOOL /*fPlayFromStart*/)
{
	HRESULT hr = S_OK;
	if(m_pDummyInstrument)
	{	
		ASSERT(m_pWaveNode);
		CDLSComponent* pComponent = m_pWaveNode->GetComponent();
		ASSERT(pComponent);

		m_pDummyInstrument->UpdatePatch();
		hr = pComponent->PlayMIDIEvent(MIDI_NOTEON,
									  (BYTE)m_rWSMP.usUnityNote, // Use wave's unity note
									  DEFAULT_VELOCITY,
									  5, // Time
									  m_pDummyInstrument->IsDrum());   

		
		// figure out latency
		REFERENCE_TIME rtLatency, rtNow;
		MUSIC_TIME mtNow;
		pComponent->m_pIPerformance->GetLatencyTime(&rtLatency);
		pComponent->m_pIPerformance->GetTime(&rtNow, &mtNow);

		DWORD lLatencyMS = (DWORD)((rtLatency - rtNow) / 10000);

		// Record when wave started playing, taking into account latency
		m_nTimePlayed = ::GetTickCount() + lLatencyMS;

		// Remember what note was sent to trigger play.
		// This will be used when stop is clicked. We don't need to specify it here
		// because this stop is sent immediately.
		m_sUnityNoteUsedToTriggerPlay = m_rWSMP.usUnityNote;

		if (SUCCEEDED(hr))
		{
			// Set playing flag to true
			m_bIsPlaying = TRUE;
		}
	}

	return hr;
}

void CWave::ReleaseDummyInstruments()
{
	if(m_pDummyInstrument)
	{
		if(m_pDummyInstrument->Release() == 0)
		{
			m_pDummyInstrument = NULL;
		}
	}
}

BOOL CWave::IsStreamingWave()
{
	return m_bStreaming;
}

BOOL CWave::IsNoPrerollWave()
{
	return m_bNoPreroll;
}

void CWave::SetStreamingWave(BOOL bStream)
{
	ASSERT(m_pWaveNode);
	if(m_pWaveNode == NULL)
	{
		return;
	}

	// Can't set this on a wave in a collection
	// Shouldn't even get here
	if(m_pWaveNode->IsInACollection())
	{
		return;
	}

	m_bStreaming = bStream;
	if(SUCCEEDED(UpdateHeaderStream()))
	{
		m_pWaveNode->ReloadDirectSoundWave();
		NotifyWaveChange(false);
	}
}

void CWave::SetNoPrerollWave(BOOL bNoPreroll)
{
	ASSERT(m_pWaveNode);
	if(m_pWaveNode == NULL)
	{
		return;
	}

	// Can't set this on a wave in a collection
	// Shouldn't even get here
	if(m_pWaveNode->IsInACollection())
	{
		return;
	}

	m_bNoPreroll = bNoPreroll;
	if(SUCCEEDED(UpdateHeaderStream()))
	{
		m_pWaveNode->ReloadDirectSoundWave();
		NotifyWaveChange(false);
	}
}


DWORD CWave::GetReadAheadTime()
{
	return m_dwReadAheadTime;
}

void CWave::SetReadAheadTime(DWORD dwReadAheadTime)
{
	ASSERT(m_pWaveNode);
	if(m_pWaveNode == NULL)
	{
		return;
	}

	// Can't set this on a wave in a collection
	// Shouldn't even get here
	if(m_pWaveNode->IsInACollection())
	{
		return;
	}

	m_dwReadAheadTime = dwReadAheadTime;
	if(SUCCEEDED(UpdateHeaderStream()))
	{
		m_pWaveNode->ReloadDirectSoundWave();
		NotifyWaveChange(false);
	}
}


HRESULT CWave::GetStreamingWaveHeader(_DMUS_IO_WAVE_HEADER* pStreamingWaveHeader)
{
	ASSERT(pStreamingWaveHeader);
	if(pStreamingWaveHeader == NULL)
	{
		return E_POINTER;
	}
	
	// Convert from milliseconds to REFERENCE_TIME
	pStreamingWaveHeader->rtReadAhead = (__int64)((double)m_dwReadAheadTime * 10000);
	pStreamingWaveHeader->dwFlags = m_bStreaming != FALSE ? DMUS_WAVEF_STREAMING : 0;
	if( m_bNoPreroll )
	{
		pStreamingWaveHeader->dwFlags |= DMUS_WAVEF_NOPREROLL;
	}

	return S_OK;
}


void CWave::RememberLoopBeforeCompression()
{
	// Copy the m_rWLOOP struct into m_rWLOOPUncompressed
	ZeroMemory(&m_rWLOOPUncompressed, sizeof(WLOOP));
	CopyMemory(&m_rWLOOPUncompressed, &m_rWLOOP, sizeof(WLOOP));
}


void CWave::RevertToLoopBeforeCompression()
{
	CopyMemory(&m_rWLOOP, &m_rWLOOPUncompressed, sizeof(WLOOP));
    ValidateWLoop(&m_rWLOOP);
	CopyWSMPToSMPL();
}


void CWave::RememberLoopAfterCompression()
{
	// Copy the m_rWLOOP struct into m_rWLOOPCompressed
	ZeroMemory(&m_rWLOOPCompressed, sizeof(WLOOP));
	CopyMemory(&m_rWLOOPCompressed, &m_rWLOOP, sizeof(WLOOP));
}

/* reverts to the saved decompressed start and loop points */
void CWave::RevertToAfterCompression() 
{
	// retrieve loop settings
	CopyMemory(&m_rWLOOP, &m_rWLOOPCompressed, sizeof(WLOOP));

	// make sure Decompressed Start is within bounds
	if (m_dwDecompressedStart > m_dwWaveLength)
		m_dwDecompressedStart = 0;

	// make sure loop are within bounds
    ValidateWLoop(&m_rWLOOP);
	CopyWSMPToSMPL();
}

void CWave::ValidateWLoop(WLOOP* pWLOOP)
{
    ASSERT(pWLOOP);
    if(pWLOOP == NULL)
    {
        return;
    }

	DWORD dwLoopMax = GetDwSelMax();
    if(pWLOOP->ulStart + pWLOOP->ulLength >= dwLoopMax)
    {
        pWLOOP->ulLength = dwLoopMax - pWLOOP->ulStart;
    }

    if(pWLOOP->ulStart + pWLOOP->ulLength <= MINIMUM_VALID_LOOPLENGTH)
    {
        pWLOOP->ulLength = MINIMUM_VALID_LOOPLENGTH;
    }
}

HRESULT	CWave::OnWaveBufferUpdated()
{
	ASSERT(m_pDataManager);
	if(m_pDataManager == NULL)
		return E_FAIL;

	// Update the header info
	m_pDataManager->UpdateHeaderInfo();

	// notify of wave changes
	NotifyWaveChange(true);

	return S_OK; 
}

void CWave::SetDirtyFlag()
{
	ASSERT(m_pWaveNode);
	if(m_pWaveNode)
	{
		if(m_pWaveNode->IsInACollection())
		{
			ASSERT(m_pCollection);
			if(m_pCollection)
			{
				m_pCollection->SetDirtyFlag();
				return;
			}
		}
		m_pWaveNode->SetDirtyFlag();
	}
}

void CWave::ClearDirtyFlag()
{
	ASSERT(m_pWaveNode);
	if(m_pWaveNode)
	{
		if(m_pWaveNode->IsInACollection())
		{
			ASSERT(m_pCollection);
			if(m_pCollection)
			{
				m_pCollection->ClearDirtyFlag();
				return;
			}
		}
		m_pWaveNode->ClearDirtyFlag();
	}
}


CString	CWave::SampleToTime(DWORD dwSample)
{
	DWORD dwSampleRate = m_rWaveformat.nSamplesPerSec;
	DWORD dwSamplesPerMillisec = dwSampleRate / 1000;
	DWORD dwSamplesPerMinute = dwSampleRate * 60;
	DWORD dwSamplesPerHour = dwSamplesPerMinute * 60;
	
	int nMillisecOffset = (dwSample / dwSamplesPerMillisec) % 1000;
	int nSecondOffset = (dwSample / dwSampleRate) % 60;
	int nMinuteOffset = (dwSample / dwSamplesPerMinute) % 60;
	int nHourOffset = (dwSample / dwSamplesPerHour) % 60;
	
	CString sTime;
	sTime.Format("%02d.%03d", nSecondOffset, nMillisecOffset);
	
	if(nMinuteOffset > 0)
	{
		CString sMinuteTime;
		sMinuteTime.Format("%02d:%s", nMinuteOffset, sTime);
		sTime = sMinuteTime;
	}

	if(nHourOffset > 0)
	{
		CString sHourTime;
		sHourTime.Format("%02d:%s", nHourOffset, sTime);
		sTime = sHourTime;
	}

	return sTime;
}

GUID CWave::GetFileGUID()
{
	return m_pWaveNode->GetFileGUID();
}

void CWave::SetFileGUID(GUID guidNew)
{
	ASSERT(m_pWaveNode);
	if(m_pWaveNode == NULL)
	{
		return;
	}

	CDLSComponent* pComponent = m_pWaveNode->GetComponent();
	ASSERT(pComponent);
	if(pComponent == NULL)
	{
		return;
	}


	IDMUSProdFramework* pIFramework = pComponent->m_pIFramework;
	ASSERT(pIFramework);
	if(pIFramework == NULL)
	{
		return;
	}

	m_pWaveNode->SetFileGUID(guidNew);

	// Notify the nodes that this nodes GUID changed
	IDMUSProdNode* pIDocRootNode = NULL;
	if(SUCCEEDED(m_pWaveNode->GetDocRootNode(&pIDocRootNode)))
	{
		pIFramework->NotifyNodes(pIDocRootNode, DOCROOT_GuidChange, NULL);
		pIDocRootNode->Release();
	}
}

HRESULT	CWave::SaveUndoState(UINT uUndoStrID)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pDataManager);
	if(m_pDataManager == NULL)
		return E_FAIL;

	CString	csMenuText;
 	csMenuText.LoadString(uUndoStrID);
	return m_pDataManager->SaveUndoState(csMenuText);
}

USHORT CWave::GetUnityNote()
{
	return m_rWSMP.usUnityNote;
}

void CWave::SetWaveID(DWORD dwID)
{
	m_dwId = dwID;
}

DWORD CWave::GetWaveID() const
{
	return m_dwId;
}

bool CWave::GetWaveForID(DWORD dwID)
{
	return (m_dwId == dwID);
}

// =========================================================================
// Creates a dummy instrument that will be used to play the wave.
// Each collection has a unique id, which is decremented by 1 for every 
// subsequent collection so that all collections that are downloaded have
// a unique id for its instruments. (pc, msb,lsb)
// =========================================================================
void CWave::SetupDummyInstrument()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// If we already have a dummy instrument release the old one and create a new one
	if(m_pDummyInstrument)
	{
		return;
	}

	ASSERT(m_pWaveNode);
	CDLSComponent* pComponent = m_pWaveNode->GetComponent();
	ASSERT(pComponent);

	// If m_pDummyInstrument is null when we try to play we'll put up an error message.
    m_pDummyInstrument = new CInstrument(pComponent);
    if(m_pDummyInstrument) 
    { 
		// Check for default articulation
		CArticulation* pArticulation = m_pDummyInstrument->GetCurrentArticulation();
        if(pArticulation)
        {
            CRegion * pRegion = new CRegion(pComponent);
            if(pRegion)
            {
				BYTE DummyMSB = 0;
				BYTE DummyLSB = 0;
				BYTE DummyPatch = 0;

                // Set the msb, lsb and pc of this instrument.
                // Note: This must be a unique number so that the collections are unique also.
				pComponent->GetNextValidDummyPatch(m_pDummyInstrument, &DummyMSB, &DummyLSB, &DummyPatch);

                m_pDummyInstrument->m_rInstHeader.Locale.ulBank = MAKE_BANK(0, DummyMSB, DummyLSB);
                m_pDummyInstrument->m_rInstHeader.Locale.ulInstrument = (ULONG) DummyPatch;

                // If there is looping set in the wave, the release should be 0
                // because then the playing will be stopped by a Stop command.
                memcpy(&(pRegion->m_rWSMP), &(m_rWSMP), sizeof(WSMPL));
                if(m_bPlayLooped)
                {   
                    // Copy the wloop structure
                    memcpy(&(pRegion->m_rWLOOP), &(m_rWLOOP), sizeof(WLOOP));
					ArticParams* pArticParams = pArticulation->GetArticParams();
                    pArticParams->m_VolEG.m_tcRelease = Mils2TimeCents(0);
                }
                else
                {
                    pRegion->m_rWSMP.cSampleLoops = 0;
					ArticParams* pArticParams = pArticulation->GetArticParams();
                    pArticParams->m_VolEG.m_tcRelease = Mils2TimeCents(0);
                }
                    
                // Add wave to region
                pRegion->m_pWave = this;
				m_pWaveNode->AddRef();

				pRegion->m_rWaveLink.fusOptions = (GetChannelCount() > 1) ? F_WAVELINK_MULTICHANNEL : 0;
			#ifdef _DEBUG
				pRegion->m_rWaveLink.ulTableIndex = 0xDEAD;
			#endif

                // Add the instrument to the region
                pRegion->m_pInstrument = m_pDummyInstrument;

                // Add the region to the instruments' list of regions.
                m_pDummyInstrument->m_Regions.AddTail(pRegion);
				
				// Download Instrument
				POSITION position = pComponent->m_lstDLSPortDownload.GetHeadPosition();
				while(position)
				{
					IDirectMusicPortDownload* pIDMPortDownload = pComponent->m_lstDLSPortDownload.GetNext(position);
					ASSERT(pIDMPortDownload);

					if(pIDMPortDownload)
					{
						m_pDummyInstrument->DM_Init(pIDMPortDownload);
						m_pDummyInstrument->Download(pIDMPortDownload);
					}
				}
				
                // Update Patch
                m_pDummyInstrument->UpdatePatch();          
            } 
        } 
    }
}

void CWave::UpdateLoopInRegions()
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
    
	// Go thru all regions and change values in the regions that
    // refer to the wave being edited if the override flag is not set.
    if(m_pCollection == NULL)
		return;

    CInstrument * pInstrument = m_pCollection->m_Instruments.GetHead();
    while (pInstrument) // for all instruments go thru all regions
    {
    	bool bInstrumentNeedsUpdate = false;
        CRegion * pRegion = pInstrument->m_Regions.GetHead();
        while (pRegion)
        {
            if (IsReferencedBy(pRegion->m_pWave))
            {
                // is override Playback set?
                if (!pRegion->m_bOverrideWaveMoreSettings)
                {
                    pRegion->m_rWSMP.cSampleLoops = pRegion->m_pWave->GetWSMPL().cSampleLoops;
                    pRegion->m_rWLOOP = pRegion->m_pWave->GetWLOOP();
                    bInstrumentNeedsUpdate = true;
                }
            }
            pRegion = pRegion->GetNext();
        } // while pregion

		// update instrument if we changed any of its regions
        if (bInstrumentNeedsUpdate)
			pInstrument->UpdateInstrument();

        pInstrument = pInstrument->GetNext();
    }
}

/* returns whether the passed-in wave points to this one or any of its channels */
bool CWave::IsReferencedBy(CWave *pWave)
{
	// check wave
	if (pWave == this)
		return true;

	// check all channels (except for mono, where wave == channel)
	LONG cChannels = GetChannelCount();
	if (cChannels > 1)
		for (LONG iChannel = 0; iChannel < cChannels; iChannel++)
			if (GetChannel(iChannel) == pWave)
				return true;

	return false;
}

void CWave::UpdateRefRegions()
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);

    // Go thru all regions and update the instrument that owns the regions that
    // refer to the wave being edited.
	if(m_pCollection == NULL)
	{
		return;
	}

    CInstrument * pInstrument = m_pCollection->m_Instruments.GetHead();
    while (pInstrument) // for all instruments go thru all regions
    {
        CRegion * pRegion = pInstrument->m_Regions.GetHead();
        while (pRegion)
        {
            if (IsReferencedBy(pRegion->m_pWave))
            {
                // updatesynth. 
                ASSERT(pRegion->m_pInstrument);
        		pRegion->m_pInstrument->UpdateInstrument();
				break;
            }
            pRegion = pRegion->GetNext();
        } // while pregion
        pInstrument = pInstrument->GetNext();
    }
}

void CWave::UpdateDummyInstrument()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_pDummyInstrument == NULL)
	{
		return;
	}

    // Copy the wsmp and smpl regions from the wave to the region in the collection.
    CRegion* pRegion = m_pDummyInstrument->m_Regions.GetHead();
    ASSERT(pRegion);

	// Copy the wsmp structure
	memcpy(&(pRegion->m_rWSMP), &(m_rWSMP), sizeof(WSMPL));
	
	// Copy the wloop structure
	memcpy(&(pRegion->m_rWLOOP), &(m_rWLOOP), sizeof(WLOOP));
  
    // Now download
	if(m_dwWaveLength > 0)
	{
		m_pDummyInstrument->UpdateInstrument();
	}
}

/* returns whether decompressed start should be taken into account for displaying or playing */
bool CWave::FConsiderDecompressedStart()
{
/* TODO: Re-enable this when we support decompressed start again
#ifndef DMP_XBOX
	// controls are enabled only if wave is compressed, and in a DLS collection
	bool bConsider = false;
	if (m_bCompressed)
		{
		CWaveNode* pWaveNode = GetNode();
		ASSERT(pWaveNode);
		if ((pWaveNode != NULL) && pWaveNode->IsInACollection())
			bConsider = true;
		}

	return bConsider;
#else
*/
	return false;
/*
#endif
*/
}

/* UI wrapper that attempts to set the decompressed start to the given value */
HRESULT CWave::HrSetDecompressedStart(DWORD dwDecompressedStart)
{
	ASSERT(FConsiderDecompressedStart()); // this shouldn't be called

	// range checking
	int intUpper = min(m_dwWaveLength, UD_MAXVAL);
	if (dwDecompressedStart > (DWORD)intUpper)
		dwDecompressedStart = (DWORD)intUpper;

	// ensure loop values are high enough
	bool fChanged = false;
	WLOOP waveLoop = GetWLOOP();
	DWORD dwLoopStart = waveLoop.ulStart + m_dwDecompressedStart;
	DWORD dwLoopEnd = dwLoopStart + waveLoop.ulLength;
	if (dwLoopStart < dwDecompressedStart)
		{
		dwLoopStart = dwDecompressedStart;
		fChanged = true;
		}
	if (dwLoopEnd < dwDecompressedStart)
		{
		dwLoopEnd = dwDecompressedStart;
		fChanged = true;
		}
	if (dwLoopEnd < dwLoopStart)
		{
		dwLoopEnd = dwLoopStart;
		fChanged = true;
		}

	// bail if no change
	if ((dwDecompressedStart == m_dwDecompressedStart) && !fChanged)
		return S_FALSE;

	// save the undo state
	if (FAILED(SaveUndoState(IDS_UNDO_DECOMPRESSEDSTART)))
		return E_FAIL;

	// update wave data, offset selection and loop to new decompressed start
	long nDelta = dwDecompressedStart - m_dwDecompressedStart;
	m_dwDecompressedStart = dwDecompressedStart;
	waveLoop.ulStart = dwLoopStart - dwDecompressedStart;
	waveLoop.ulLength = dwLoopEnd - dwLoopStart;
	SetWLOOP(waveLoop);

	// Copy the loop information to the legacy structure
	CopyWSMPToSMPL();

	// the compressed loop also gets modified
	RememberLoopAfterCompression();

	// update header info (also notified of change and sets dirty flag)
	if (FAILED(OnWaveBufferUpdated()))
		{
		PopUndoState();
		return E_FAIL;
		}

	// update synth, and all regions referencing this wave
	UpdateWave();
	UpdateLoopStartAndLengthInRegions();
	UpdateDummyInstrument();

	CDLSComponent* pComponent = NULL;
	CWaveNode* pWaveNode = GetNode();
	if(pWaveNode)
		pComponent = pWaveNode->GetComponent();
	else
		{
		ASSERT(FALSE); // missing component
		pComponent = NULL;
		}	

	// offset selection in wave editor, and redraw
	CWaveCtrl* pWaveEditor = GetWaveEditor();
	if (pWaveEditor != NULL)
		{
		int nSelStart, nSelEnd;
		pWaveEditor->GetSelection(nSelStart, nSelEnd);
		nSelStart -= nDelta;
		nSelEnd -= nDelta;
		pWaveEditor->SetSelection(nSelStart, nSelEnd, (pComponent != NULL) && pComponent->IsSnapToZero());
		pWaveEditor->InvalidateRect(NULL);
		}

	// refresh property page
	if (pComponent != NULL)
		{
		IDMUSProdPropSheet* pIPropSheet;
		if (SUCCEEDED(pComponent->m_pIFramework->QueryInterface(IID_IDMUSProdPropSheet, (void**)&pIPropSheet)))
			{
			if(pIPropSheet->IsShowing() == S_OK)
				OnShowProperties();
			pIPropSheet->Release();
			}
		}

	return S_OK;
}

/* UI wrapper that validates loop start and end. Caller should only save undo state and set loop values on success. */
HRESULT CWave::HrValidateLoopValues(DWORD &dwLoopStart, DWORD &dwLoopLength, const bool bLockLoopLength)
{
	HRESULT hr = S_OK;
	WLOOP waveLoop = GetWLOOP();

#if MINIMUM_VALID_LOOPSTART > 0
	// If the start of the loop is less than the minimum valid we set it right...
	if (dwLoopStart < MINIMUM_VALID_LOOPSTART)
		{
		dwLoopStart = MINIMUM_VALID_LOOPSTART;
		hr = E_FAIL;
		}
#endif

	// If the start value exceeds the maximum possible value 
	// it's an error so we set it right back to the old value
	DWORD dwLoopMax = GetDwSelMax();
	if (dwLoopStart > dwLoopMax - MINIMUM_VALID_LOOPLENGTH)
		{
		AfxMessageBox(IDS_ERR_INVALID_LOOPSTART, MB_ICONEXCLAMATION);

		dwLoopStart = waveLoop.ulStart;
		hr = E_FAIL;
		}

	// If the loop length is less than the minimum valid we set it right...
	if (dwLoopLength < MINIMUM_VALID_LOOPLENGTH)
		{
		AfxMessageBox(IDS_LOOP_TOO_SMALL, MB_ICONEXCLAMATION);

		dwLoopLength = waveLoop.ulLength;
		hr  = E_FAIL;
		}

	// If the length of the loop exceeds the total wavelength and the loop length is *NOT LOCKED*
	// then we set it to the maximum possible looplength
	if (dwLoopStart + dwLoopLength > dwLoopMax)
		{
		if (!bLockLoopLength)
			dwLoopLength = dwLoopMax - dwLoopStart;
		else
			dwLoopStart = waveLoop.ulStart;

		hr = E_FAIL;
		}

	if (dwLoopLength >= MSSYNTH_LOOP_LIMIT)
		{
		int nChoice = AfxMessageBox(IDS_WARN_MSSYNTH_LOOP_LIMIT, MB_OKCANCEL);
		if (nChoice == IDCANCEL)
			{
			dwLoopLength = MSSYNTH_LOOP_LIMIT - 1;
			hr = E_FAIL;
			}
		}

	return hr;
}

/* loop start has changed in wave, update all regions referencing the wave */
void CWave::UpdateLoopStartAndLengthInRegions()
{
    // Go thru all regions and chenge values in the regions that
    // refer to the wave being edited if the override flag is not set.
	CCollection* pCollection = GetCollection();
	if(!pCollection)
		return;

	WLOOP waveLoop = GetWLOOP();

    CInstrument * pInstrument = pCollection->m_Instruments.GetHead();

    while (pInstrument) // for all instruments go thru all regions
    {
        CRegion * pRegion = pInstrument->m_Regions.GetHead();
        while (pRegion)
        {
            if (pRegion->m_pWave == this)
            {
                // is override Playback set?
                if (!pRegion->m_bOverrideWaveMoreSettings)
                {
                    pRegion->m_rWLOOP.ulStart = waveLoop.ulStart;
                    pRegion->m_rWLOOP.ulLength = waveLoop.ulLength;
                    // updatesynth. 
                    ASSERT(pRegion->m_pInstrument);
        			pRegion->m_pInstrument->UpdateInstrument();
                }
            }
            pRegion = pRegion->GetNext();
        } // while pregion
        pInstrument = pInstrument->GetNext();
    }
}

/* ensures the given sample count is within selection boundaries */
void CWave::ConstrainToSelectionBoundaries(int& nSel)
{
	DWORD dwDecompressedStart = GetDwDecompressedStart(true);
	int nSelMin = -(int)(dwDecompressedStart);
	int nSelMax = m_dwWaveLength-dwDecompressedStart-1;

	if (nSel < nSelMin)
		nSel = nSelMin;
	else if (nSel > nSelMax)
		nSel = nSelMax;
}
