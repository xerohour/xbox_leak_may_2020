#include <windows.h>
#include "dumpp.h"


extern long g_cComponent;

STDAPI_(HRESULT) MMRESULTtoHRESULT(MMRESULT mmr)
{
	HRESULT hr;

	switch(mmr)
	{
	case MMSYSERR_NOERROR:
		hr = DS_OK;
		break;

	case MMSYSERR_BADDEVICEID:
	case MMSYSERR_NODRIVER:
		hr = DSERR_NODRIVER;
		break;

	case MMSYSERR_ALLOCATED:
		hr = DSERR_ALLOCATED;
		break;

	case MMSYSERR_NOMEM:
		hr = DSERR_OUTOFMEMORY;
		break;

	case MMSYSERR_NOTSUPPORTED:
		hr = DSERR_UNSUPPORTED;
		break;

	case WAVERR_BADFORMAT:
		hr = DSERR_BADFORMAT;
		break;

	default:
		hr = DSERR_GENERIC;
		break;
	}
	
	return hr;
}

STDMETHODIMP CDirectSoundDumpDMO::QueryInterface(THIS_ REFIID iid, LPVOID *ppv)
{
	if (ppv == NULL)
	{
		return E_POINTER;
	}

	if (iid == IID_IUnknown || iid == IID_IPersist)
	{
		*ppv = (LPVOID*)static_cast<IPersist*>(this);
	}
	else if (iid == IID_IPersistStream)
	{
		*ppv = (LPVOID*)static_cast<IPersistStream*>(this);
	}
	else if (iid == IID_IMediaObject)
	{
		*ppv = (LPVOID*)static_cast<IMediaObject*>(this);
	}
	else if (iid == IID_IMediaObjectInPlace)
	{
		*ppv = (LPVOID*)static_cast<IMediaObjectInPlace*>(this);
	}
	else if (iid == IID_IDump)
	{
		*ppv = (LPVOID*)static_cast<IDump*>(this);
	}
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

STDMETHODIMP_(ULONG) CDirectSoundDumpDMO::AddRef(THIS)
{
	return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CDirectSoundDumpDMO::Release(THIS)
{
	LONG l = InterlockedDecrement(&m_cRef);

	if (l == 0) 
	{
		delete this;
		return 0;
	}

	return l;
}

CDirectSoundDumpDMO::CDirectSoundDumpDMO() :
m_fDirty(FALSE),
m_bInitialized(FALSE),
m_fOverwriteExisting(FALSE),
m_cRef(1),
m_hmmioDst(NULL)
{
	InterlockedIncrement(&g_cComponent);

	wcscpy( m_wszDumpWave, DEFAULT_DUMP_FILE );

	::InitializeCriticalSection( &m_csFileHandle );
}

CDirectSoundDumpDMO::~CDirectSoundDumpDMO()
{
	if ( m_hmmioDst )
	{
		if (MMSYSERR_NOERROR == mmioAscend(m_hmmioDst, &m_ckDst, 0))
		{
			if (MMSYSERR_NOERROR == mmioAscend(m_hmmioDst, &m_ckDstRIFF, 0))
			{
				mmioClose(m_hmmioDst, 0);
			}
		}
	}
	m_hmmioDst = 0;

	::DeleteCriticalSection( &m_csFileHandle );

	InterlockedDecrement(&g_cComponent);
}

HRESULT CDirectSoundDumpDMO::Init()
{
	HRESULT hr = DS_OK;

	m_bInitialized = TRUE;
	return hr;
}

HRESULT CDirectSoundDumpDMO::Discontinuity()
{
	return NOERROR;
}

HRESULT CDirectSoundDumpDMO::FBRProcess(DWORD cSamples, BYTE *pIn, BYTE *pOut)
{
	HRESULT 		hr = DS_OK;
	LONG			lOut = cSamples * m_cChannels * (m_b8bit ? 1 : 2);

	if (!m_bInitialized)
		return DMO_E_TYPE_NOT_SET;

	memcpy(pOut, pIn, cSamples * m_cChannels * (m_b8bit ? 1 : 2));

	::EnterCriticalSection( &m_csFileHandle );
	if ( m_hmmioDst )
	{
		if ( lOut != mmioWrite(m_hmmioDst, (HPSTR) pIn, lOut) )
		{
			hr = DSERR_GENERIC;
		}
	}
	::LeaveCriticalSection( &m_csFileHandle );

	return hr;
}


// GetClassID
//
// Part of the persistent file support.  We must supply our class id
// which can be saved in a graph file and used on loading a graph with
// a dump in it to instantiate this filter via CoCreateInstance.
//
HRESULT CDirectSoundDumpDMO::GetClassID(CLSID *pClsid)
{
	if (pClsid==NULL) {
		return E_POINTER;
	}
	*pClsid = GUID_DSFX_STANDARD_DUMP;
	return NOERROR;

} // GetClassID


//
// GetSizeMax
//
// State the maximum number of bytes we would ever write in a file
// to save our properties.
//
HRESULT CDirectSoundDumpDMO::GetSizeMax( ULARGE_INTEGER *pcbSize )
{
	if( !pcbSize )
	{
		return E_POINTER;
	}

	// dump wave file name and overwrite flag
	pcbSize->QuadPart = _MAX_PATH*sizeof(WCHAR) + sizeof(BOOL);
	return S_OK;

}  // GetSizeMax

//
// Save
//
// Write our properties to the stream.
//
HRESULT CDirectSoundDumpDMO::Save(IStream *pStm, BOOL fClearDirty)
{
	DWORD cbWritten;
	HRESULT hr = pStm->Write( &m_fOverwriteExisting, sizeof(BOOL), &cbWritten );
	if( cbWritten != sizeof(BOOL) )
	{
		hr = E_FAIL;
	}
	if( SUCCEEDED( hr ) )
	{
		hr = pStm->Write(&(m_wszDumpWave[0]), _MAX_PATH*sizeof(WCHAR), &cbWritten);
		if( cbWritten != _MAX_PATH*sizeof(WCHAR) )
		{
			hr = E_FAIL;
		}
	}

	if( SUCCEEDED( hr )
		&&	fClearDirty )
	{
		m_fDirty = FALSE;
	}

	return hr;
} // Save


//
// Load
//
// Read our properties from the stream.
//
HRESULT CDirectSoundDumpDMO::Load(IStream *pStream)
{
	DWORD cbRead;
	HRESULT hr = pStream->Read( &m_fOverwriteExisting, sizeof(BOOL), &cbRead );
	if( cbRead != sizeof(BOOL) )
	{
		hr = E_FAIL;
	}
	if( SUCCEEDED( hr ) )
	{
		hr = pStream->Read(&(m_wszDumpWave[0]), _MAX_PATH*sizeof(WCHAR), &cbRead);
		if( cbRead != _MAX_PATH*sizeof(WCHAR) )
		{
			hr = E_FAIL;
		}
	}

	return hr;
} // Load

HRESULT CDirectSoundDumpDMO::IsDirty(void)
{
	return ( m_fDirty ) ? S_OK : S_FALSE;
}


// ==============Implementation of the private IDump interface ==========

void GetUniqueFilename( CHAR szDumpWave[_MAX_PATH] )
{
	// If the file exists
	if( mmioOpen( szDumpWave, NULL, MMIO_EXIST ) )
	{
		// Copy the original name
		CHAR szOrigName[_MAX_PATH];
		strcpy( szOrigName, szDumpWave );

		// Find the position of the last '.'
		CHAR *pDot = strrchr( szOrigName, '.' );

		// If we didn't find the dot
		if( pDot == NULL )
		{
			// Point pDot at the trailing NULL
			pDot = szOrigName + strlen( szOrigName );
		}
		else
		{
			// Cut off the name at the dot
			*pDot = NULL;
		}

		// The string to store the number in
		CHAR *pNumberStart = pDot-1;
		CHAR szNbr[_MAX_PATH];
		while( pNumberStart > szOrigName  &&  isdigit( *pNumberStart ) )
		{
			pNumberStart--;
		}

		// Increment to the first number
		*pNumberStart++;

		// Check if we didn't find a number
		if( pNumberStart == pDot )
		{
			// No number - make szNbr empty
			szNbr[0] = NULL;
		}
		else
		{
			// Copy the number to szNbr
			strcpy( szNbr, pNumberStart );

			// Cut off the name at the start of the number
			*pNumberStart = NULL;
		}

		// Save the length of the textual part of the original name
		const int nOrigNameLength = strlen( szOrigName );

		// Set the first number to use
		int i = atoi( szNbr );

		// While the filename exists
		while( mmioOpen( szDumpWave, NULL, MMIO_EXIST ) )
		{
			// Convert from a number to a string
			_itoa( ++i, szNbr, 10 );

			// Get the length of the number text
			int nNbrLength = strlen(szNbr);

			// Ensure the name doesn't go longer than _MAX_PATH (5 = length of ".wav" plus the trailing NULL)
			if( (nOrigNameLength + nNbrLength + 5) <= _MAX_PATH )
			{
				// Name will fit within _MAX_PATH, just copy it
				strcpy( szDumpWave, szOrigName );
				strcat( szDumpWave, szNbr );
				strcat( szDumpWave, ".wav" );
			}
			else
			{
				// Name won't fit within _MAX_PATH, clip szOrigName
				ZeroMemory( szDumpWave, _MAX_PATH );
				strncpy( szDumpWave, szOrigName, _MAX_PATH - nNbrLength - 5 );
				strcat( szDumpWave, szNbr );
				strcat( szDumpWave, ".wav" );
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
//
// CDirectSoundDumpDMO::InitWave
//
HRESULT CDirectSoundDumpDMO::InitWave(WCHAR *wszDumpWave)
{
	HRESULT 			hr = DS_OK;
	MMRESULT			mmr;

	CHAR				szDumpWave[_MAX_PATH];

	HMMIO				hmmioDst = NULL;
	MMCKINFO			ckDst;
	MMCKINFO			ckDstRIFF;

	PCMWAVEFORMAT		wfx;

	if (wszDumpWave == NULL)
	{
		return E_POINTER;
	}

	::EnterCriticalSection( &m_csFileHandle );
	if ( m_hmmioDst )
	{
		if (MMSYSERR_NOERROR == mmioAscend(m_hmmioDst, &m_ckDst, 0))
		{
			if (MMSYSERR_NOERROR == mmioAscend(m_hmmioDst, &m_ckDstRIFF, 0))
			{
				mmioClose(m_hmmioDst, 0);
			}
		}
	}

	m_hmmioDst = NULL;
	::LeaveCriticalSection( &m_csFileHandle );

	if( !m_bInitialized )
	{
		return E_FAIL;
	}

	WideCharToMultiByte(CP_ACP, 0, wszDumpWave, -1, szDumpWave, sizeof(szDumpWave), NULL, NULL);

	if( !m_fOverwriteExisting )
	{
		GetUniqueFilename( szDumpWave );
	}

	hmmioDst = mmioOpen( szDumpWave, NULL, MMIO_CREATE | MMIO_WRITE | MMIO_EXCLUSIVE | MMIO_ALLOCBUF );
	if (NULL == hmmioDst )
	{
		return E_FAIL;
	}

	ckDstRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	ckDstRIFF.cksize  = 0L;
	mmr = mmioCreateChunk(hmmioDst, &ckDstRIFF, MMIO_CREATERIFF);
	if (MMSYSERR_NOERROR != mmr)
	{
		hr = MMRESULTtoHRESULT(mmr);
	}

	if ( SUCCEEDED(hr) )
	{
		ckDst.ckid		= mmioFOURCC( 'f', 'm', 't', ' ' );
		ckDst.cksize	= sizeof(PCMWAVEFORMAT);
		mmr = mmioCreateChunk( hmmioDst, &ckDst, 0 );
		if ( MMSYSERR_NOERROR != mmr )
		{
			hr = MMRESULTtoHRESULT(mmr);
		}
	}

	if ( SUCCEEDED(hr) )
	{
		wfx.wBitsPerSample			= m_b8bit ? 8 : 16;
		wfx.wf.wFormatTag			= WAVE_FORMAT_PCM;
		wfx.wf.nChannels			= (WORD) m_cChannels;
		wfx.wf.nSamplesPerSec		= m_ulSamplingRate;
		wfx.wf.nBlockAlign			= wfx.wf.nChannels * wfx.wBitsPerSample / 8;
		wfx.wf.nAvgBytesPerSec		= wfx.wf.nSamplesPerSec * wfx.wf.nBlockAlign;

		if ( sizeof(PCMWAVEFORMAT) != mmioWrite(hmmioDst, (HPSTR) &wfx, sizeof(PCMWAVEFORMAT)) )
		{
			hr = DSERR_GENERIC;
		}
	}

	if ( SUCCEEDED(hr) )
	{
		mmr = mmioAscend( hmmioDst, &ckDst, 0 );
		if ( MMSYSERR_NOERROR != mmr )
		{
			hr = MMRESULTtoHRESULT(mmr);
		}
	}

	if ( SUCCEEDED(hr) )
	{
		ckDst.ckid		= mmioFOURCC( 'd', 'a', 't', 'a' );
		ckDst.cksize	= 0;

		mmr = mmioCreateChunk( hmmioDst, &ckDst, 0 );
		if ( MMSYSERR_NOERROR != mmr )
		{
			hr = MMRESULTtoHRESULT(mmr);
		}
	}

	if (SUCCEEDED(hr))
	{
		::EnterCriticalSection( &m_csFileHandle );
		m_hmmioDst = hmmioDst;
		CopyMemory( &m_ckDst, &ckDst, sizeof(ckDst) );
		CopyMemory( &m_ckDstRIFF, &ckDstRIFF, sizeof(ckDstRIFF) );
		::LeaveCriticalSection( &m_csFileHandle );
	}

	return hr;
}

//////////////////////////////////////////////////////////////////////////////
//
// CDirectSoundDumpDMO::SetDumpWave
//
STDMETHODIMP CDirectSoundDumpDMO::SetDumpWave(THIS_ WCHAR *wszDumpWave)
{
	HRESULT 			hr = DS_OK;

	if (wszDumpWave == NULL)
	{
		return E_POINTER;
	}

	if ( (0 == _wcsicmp(wszDumpWave, m_wszDumpWave)) && m_hmmioDst )
	{
		// no need to do anything
		return DS_OK;
	}

	// If we're writing out a file
	if( m_hmmioDst )
	{
		// Close the file and open a new one with the new name
		hr = InitWave(wszDumpWave);
	}

	if (SUCCEEDED(hr))
	{
		wcscpy( m_wszDumpWave, wszDumpWave );
	}

	return hr;
}

//////////////////////////////////////////////////////////////////////////////
//
// CDirectSoundDumpDMO::GetDumpWave
//
STDMETHODIMP CDirectSoundDumpDMO::GetDumpWave(THIS_ WCHAR *wszDumpWave)
{
	if (wszDumpWave == NULL)
	{
		return E_POINTER;
	}

	wcscpy( wszDumpWave, m_wszDumpWave );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
// CDirectSoundDumpDMO::SetOverwrite
//
STDMETHODIMP CDirectSoundDumpDMO::SetOverwrite(THIS_ BOOL bOverwrite)
{
	m_fOverwriteExisting = bOverwrite;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
// CDirectSoundDumpDMO::GetOverwrite
//
STDMETHODIMP CDirectSoundDumpDMO::GetOverwrite(THIS_ BOOL *pbOverwrite)
{
	if (pbOverwrite == NULL)
	{
		return E_POINTER;
	}

	*pbOverwrite = m_fOverwriteExisting;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
// CDirectSoundDumpDMO::Start
//
STDMETHODIMP CDirectSoundDumpDMO::Start(THIS)
{
	HRESULT hr = S_FALSE;
	if( (NULL == m_hmmioDst) )
	{
		hr = InitWave( m_wszDumpWave );
	}

	return hr;
}

//////////////////////////////////////////////////////////////////////////////
//
// CDirectSoundDumpDMO::Stop
//
STDMETHODIMP CDirectSoundDumpDMO::Stop(THIS)
{
	::EnterCriticalSection( &m_csFileHandle );
	if ( m_hmmioDst )
	{
		if (MMSYSERR_NOERROR == mmioAscend(m_hmmioDst, &m_ckDst, 0))
		{
			if (MMSYSERR_NOERROR == mmioAscend(m_hmmioDst, &m_ckDstRIFF, 0))
			{
				mmioClose(m_hmmioDst, 0);
			}
		}
	}

	m_hmmioDst = NULL;
	::LeaveCriticalSection( &m_csFileHandle );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
// CDirectSoundDumpDMO::ProcessInPlace
//
HRESULT CDirectSoundDumpDMO::ProcessInPlace(ULONG ulQuanta, LPBYTE pcbData, REFERENCE_TIME rtStart, DWORD dwFlags)
{
	return FBRProcess(ulQuanta, pcbData, pcbData);
}


//////////////////////////////////////////////////////////////////////////////
//
// CDirectSoundDumpDMO::Clone
//
STDMETHODIMP CDirectSoundDumpDMO::Clone(THIS_ IMediaObjectInPlace **ppDest)
{
	HRESULT hr = S_OK;
	CDirectSoundDumpDMO *pNewDumpDMO = NULL;

	try 
	{
		pNewDumpDMO = new CDirectSoundDumpDMO;
	} catch(...) {}

	if (pNewDumpDMO == NULL) 
	{
		hr = E_OUTOFMEMORY;
	}

	if (SUCCEEDED(hr))
	{
		wcscpy( pNewDumpDMO->m_wszDumpWave, m_wszDumpWave );
		pNewDumpDMO->m_fOverwriteExisting = m_fOverwriteExisting;
	}

	if (SUCCEEDED(hr))
	{	   
		hr = pNewDumpDMO->QueryInterface(IID_IMediaObjectInPlace, (void**)ppDest); 
		pNewDumpDMO->Release();
	}							   
	else
	{
		delete pNewDumpDMO;
	}

	return hr;								 
}
