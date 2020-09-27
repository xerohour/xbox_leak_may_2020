//////////////////////////////////////////////////////////////////////////////
// CLoader

#include "stdafx.h"
#include "JazzApp.h"
#include <dmusicp.h>
#include <dsoundp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////////////
// CLoader class

CLoader::CLoader()
{
	m_dwRef = 1;
//	m_dwPRef = 0;
	m_pUnkDispatch = NULL;
	m_pIGMCollection = NULL;
}

CLoader::~CLoader()
{
	if( m_pUnkDispatch )
	{
		m_pUnkDispatch->Release();	// free IDispatch implementation we may have borrowed
	}

	if( m_pIGMCollection )
	{
		m_pIGMCollection->Release();	
	}
}
	
//	ULONG AddRefP( void )				// Private AddRef, for streams.
//	{
//      return ++m_dwPRef;
//	}
//
//	ULONG ReleaseP( void )				// Private Release, for streams.
//	{
//		if( --m_dwPRef == 0L )
//		{
//			if( m_dwRef == 0L )
//			{
//				delete this;
//				return 0;
//			}
//		}
//      return m_dwPRef;
//	}



/////////////////////////////////////////////////////////////////////////////
// CLoader IUnknown implementation

/////////////////////////////////////////////////////////////////////////////
// CLoader IUnknown::QueryInterface

HRESULT CLoader::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
    if( IsEqualIID( riid, IID_IUnknown )
    ||  IsEqualIID( riid, IID_IDirectMusicLoader ) )
    {
        *ppvObj = (IStream *)this;
        AddRef();
        return NOERROR;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}


/////////////////////////////////////////////////////////////////////////////
// CLoader IUnknown::AddRef

ULONG CLoader::AddRef()
{
    return ++m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// CLoader IUnknown::Release

ULONG CLoader::Release()
{
	ASSERT( m_dwRef != 0 );

    if( --m_dwRef == 0L )
    {
//		m_dwRef = 100;	// artificial ref count to prevent reentrency due to COM aggregation
//		if( m_dwPRef == 0L )
//		{
	        delete this;
		    return 0;
//		}
    }
    return m_dwRef;
}



/////////////////////////////////////////////////////////////////////////////
// CLoader IDirectMusicLoader methods

/////////////////////////////////////////////////////////////////////////////
// CLoader IDirectMusicLoader::GetObject

HRESULT CLoader::GetObject( LPDMUS_OBJECTDESC pDesc, REFIID riid, LPVOID FAR * ppVoid )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ppVoid == NULL )
	{
		return E_POINTER;
	}

	*ppVoid = NULL;

	if( pDesc == NULL )
	{
		return E_INVALIDARG;
	}

	// Must have a CLSID
	if( !( pDesc->dwValidData & DMUS_OBJ_CLASS ) )
	{
		return E_FAIL;
	}

	// We need either a GUID or a stream
	if( !( pDesc->dwValidData & (DMUS_OBJ_OBJECT | DMUS_OBJ_STREAM ) ) )
	{
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;

	if( pDesc->dwValidData & DMUS_OBJ_OBJECT )
	{
		// Handle request for GM Collection
		
		if( ::IsEqualGUID( pDesc->guidClass, CLSID_DirectMusicCollection ) 
		&&  ::IsEqualGUID( pDesc->guidObject, GUID_DefaultGMCollection ) )
		{
			hr = E_FAIL;
			if( m_pIGMCollection == NULL )
			{
				// Get the DirectMusic loader
				IDirectMusicLoader* pILoader;
				if( SUCCEEDED ( theApp.m_pFramework->GetSharedObject( CLSID_DirectMusicLoader,
																	  IID_IDirectMusicLoader, 
																	  (LPVOID*)&pILoader) ) )
				{
					// Prepare DMUS_OBJECTDESC structure
					DMUS_OBJECTDESC desc;
					memset( &desc, 0, sizeof(DMUS_OBJECTDESC) );
					desc.dwSize = sizeof(desc);
					desc.guidClass = CLSID_DirectMusicCollection;
					desc.guidObject = GUID_DefaultGMCollection;
					desc.dwValidData = (DMUS_OBJ_CLASS | DMUS_OBJ_OBJECT);

					// Get the GM collection
					if( FAILED ( pILoader->GetObject( &desc, IID_IUnknown, (void **)&m_pIGMCollection ) ) )
					{
						m_pIGMCollection = NULL;
					}

					pILoader->Release();
				}
			}
			if( m_pIGMCollection )
			{
				hr = m_pIGMCollection->QueryInterface( riid, ppVoid );
			}
		}
		else
		{
			// Use the Producer generated GUID to get the file's FileNode
			CFileNode* pFileNode = theApp.GetFileByGUID( pDesc->guidObject );

			// We found the file's FileNode
			if( pFileNode )
			{
				// Make sure the FileNode's file is loaded
				if( pFileNode->m_pIChildNode == NULL )
				{
					CString strFileName;
					pFileNode->ConstructFileName( strFileName );
					
					// Open the file
					theApp.m_nShowNodeInTree++;
					theApp.OpenTheFile( strFileName, TGT_FILENODE );
					theApp.m_nShowNodeInTree--;
				}

				if( pFileNode->m_pIChildNode )
				{
					hr = pFileNode->m_pIChildNode->GetObject( pDesc->guidClass, riid, ppVoid );

					if( FAILED ( hr ) )
					{
						GUID guidNodeId;
						if( SUCCEEDED ( pFileNode->m_pIChildNode->GetNodeId( &guidNodeId ) ) )
						{
							if( ::IsEqualGUID( pDesc->guidClass, CLSID_DirectMusicSegment ) 
							&&  ::IsEqualGUID( guidNodeId, GUID_WaveNode ) ) 
							{
								IDirectMusicSegment8* pIDMSegment8;
								hr = CreateSegmentFromWave( pFileNode->m_pIChildNode, &pIDMSegment8 );
								if( SUCCEEDED ( hr ) )
								{
									hr = pIDMSegment8->QueryInterface( riid, ppVoid );

									pIDMSegment8->Release();
								}
							}
						}
					}
				}

				pFileNode->Release();
			}
		}
	}
	else if( pDesc->dwValidData & DMUS_OBJ_STREAM )
	{
		hr = ::CoCreateInstance( pDesc->guidClass, NULL, CLSCTX_INPROC_SERVER, riid, ppVoid );
		if( SUCCEEDED ( hr ) )
		{
			IPersistStream* pIPersistStream;
			hr = ((IUnknown *) *ppVoid)->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream );
			if( SUCCEEDED ( hr ) )
			{
				IStream* pIStream = NULL;

				hr = pDesc->pStream->Clone( &pIStream );
				if( SUCCEEDED ( hr ) )
				{
					hr = pIPersistStream->Load( pIStream );
					pIStream->Release();
				}

				pIPersistStream->Release();
			}
		}
	}

	if( FAILED ( hr ) )
	{
		if( *ppVoid )
		{
			((IUnknown *) *ppVoid)->Release();
		}
		*ppVoid = NULL;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CLoader IDirectMusicLoader::SetObject

HRESULT CLoader::SetObject( LPDMUS_OBJECTDESC pDesc)
{
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CLoader IDirectMusicLoader::SetSearchDirectory

HRESULT CLoader::SetSearchDirectory( REFGUID rguidClass, WCHAR* pwzPath, BOOL fClear )
{
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CLoader IDirectMusicLoader::ScanDirectory

HRESULT CLoader::ScanDirectory( REFGUID rguidClass, WCHAR* pwzFileExtension, WCHAR* pwzScanFileName )
{
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CLoader IDirectMusicLoader::CacheObject

HRESULT CLoader::CacheObject( IDirectMusicObject* pObject )
{
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CLoader IDirectMusicLoader::ReleaseObject

HRESULT CLoader::ReleaseObject( IDirectMusicObject* pObject )
{
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CLoader IDirectMusicLoader::ClearCache

HRESULT CLoader::ClearCache( REFGUID rguidClass )
{
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CLoader IDirectMusicLoader::EnableCache

HRESULT CLoader::EnableCache( REFGUID rguidClass, BOOL fEnable )
{
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CLoader IDirectMusicLoader::EnumObject

HRESULT CLoader::EnumObject( REFGUID rguidClass, DWORD dwIndex, LPDMUS_OBJECTDESC pDesc )
{
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CLoader::CreateSegmentFromWave

HRESULT CLoader::CreateSegmentFromWave( IDMUSProdNode* pIWaveNode, IDirectMusicSegment8** ppIDMSegment8 )
{
	if( ppIDMSegment8 == NULL )
	{
		return E_POINTER;
	}
	*ppIDMSegment8 = NULL;

	if( pIWaveNode == NULL )
	{
		return E_INVALIDARG;
	}

	IDirectMusicSegment8* pIDMSegment8 = NULL;
	IDirectMusicSegment8P* pIDMSegment8P;
	IDirectSoundWave* pIDirectSoundWave;
	IDirectMusicTrack* pIDMWaveTrack;
	IPrivateWaveTrack* pIPrivateWaveTrack;

	// Get a DirectSound wave object
	HRESULT hr = pIWaveNode->GetObject( CLSID_DirectSoundWave, IID_IDirectSoundWave, (void**)&pIDirectSoundWave );
	if( SUCCEEDED ( hr ) )
	{
		// CoCreate a segment
		hr = ::CoCreateInstance( CLSID_DirectMusicSegment, NULL, CLSCTX_INPROC,
								 IID_IDirectMusicSegment8, (void**)&pIDMSegment8 );
		if( SUCCEEDED ( hr ) )
		{
			hr = pIDMSegment8->QueryInterface( IID_IDirectMusicSegment8P, (void**)&pIDMSegment8P );
			if( SUCCEEDED ( hr ) )
			{
				// CoCreate a wave track
				hr = ::CoCreateInstance( CLSID_DirectMusicWaveTrack, NULL, CLSCTX_INPROC,
										 IID_IDirectMusicTrack, (void**)&pIDMWaveTrack );
				if( SUCCEEDED ( hr ) )
				{
					// Add the wave object to the wave track, and insert the track in the segment.
					hr = pIDMWaveTrack->QueryInterface( IID_IPrivateWaveTrack, (void**)&pIPrivateWaveTrack );
					if( SUCCEEDED ( hr ) )
					{
						// Add wave track to Segment
						REFERENCE_TIME rt = 0;
						hr = pIPrivateWaveTrack->AddWave( pIDirectSoundWave, 0, 0, 0, &rt );
						if( SUCCEEDED ( hr ) )
						{
							DWORD dwSize = sizeof(DMUS_IO_SEGMENT_HEADER);
							DMUS_IO_SEGMENT_HEADER dmusSegmentHeaderIO;
							if( SUCCEEDED ( pIDMSegment8P->GetHeaderChunk( &dwSize, &dmusSegmentHeaderIO ) ) )
							{
								dmusSegmentHeaderIO.rtLength = (rt * REF_PER_MIL);
								dmusSegmentHeaderIO.dwFlags |= DMUS_SEGIOF_REFLENGTH;
								pIDMSegment8P->SetHeaderChunk( sizeof(DMUS_IO_SEGMENT_HEADER), &dmusSegmentHeaderIO );
							}
						}
						pIDMSegment8->InsertTrack( pIDMWaveTrack, 1 );
						pIDMSegment8->SetTrackConfig( CLSID_DirectMusicWaveTrack, 1, 0,
													 (DMUS_TRACKCONFIG_DEFAULT | DMUS_TRACKCONFIG_PLAY_CLOCKTIME), 0);

						pIPrivateWaveTrack->Release();
					}

					pIDMWaveTrack->Release();
				}

				pIDMSegment8P->Release();
			}
		}

		pIDirectSoundWave->Release();
	}

	if( SUCCEEDED ( hr ) )
	{
		*ppIDMSegment8 = pIDMSegment8;
	}
	else
	{
		if( pIDMSegment8 )
		{
			pIDMSegment8->Release();
		}
	}

	return hr;
}
