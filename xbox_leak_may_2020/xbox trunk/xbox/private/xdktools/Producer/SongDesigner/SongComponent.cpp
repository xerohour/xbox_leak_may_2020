// SongComponent.cpp : implementation file
//

#include "stdafx.h"

#include "SongDesignerDLL.h"
#include "Song.h"
#include "SongRef.h"
#include "DupeFileDlg.h"
#include <SegmentDesigner.h>
#include <ContainerDesigner.h>
#include <AudioPathDesigner.h>
#include <ToolGraphDesigner.h>
#include <dmusici.h>


/////////////////////////////////////////////////////////////////////////////
// CSongComponent constructor/destructor 

CSongComponent::CSongComponent()
{
    m_dwRef = 0;
	
	m_pISongDocType8 = NULL;

	m_pIFramework8 = NULL;
	m_pIDMPerformance8 = NULL;
	m_pIConductor = NULL;

	m_pISegmentComponent = NULL;
	m_pIContainerComponent = NULL;
	m_pIAudioPathComponent = NULL;
	m_pIToolGraphComponent = NULL;
	
	m_nFirstImage = 0;
	m_nNextSong = 0;

	m_dwEmbeddedFileListUseCount = 0;
	m_pIEmbeddedFileStream = NULL;
	m_pEmbeddedFileRootFile = NULL;
	m_pSongBeingLoaded = NULL;
	m_pIDupeFileTargetProject = NULL;
	m_nDupeFileDlgReturnCode = 0;

	m_cfProducerFile = 0;
	m_cfSong = 0;
	m_cfSongList = 0;
	m_cfSegment = 0;
	m_cfContainer = 0;
	m_cfAudioPath = 0;
	m_cfGraph = 0;
}

CSongComponent::~CSongComponent()
{
	ReleaseAll();
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent::ReleaseAll

void CSongComponent::ReleaseAll( void )
{
 	CDirectMusicSong* pSong;
	while( !m_lstSongs.IsEmpty() )
	{
		pSong = static_cast<CDirectMusicSong*>( m_lstSongs.RemoveHead() );
		RELEASE( pSong );
	}

 	RegisteredTrack* pRegisteredTrack;
	while( !m_lstRegisteredTracks.IsEmpty() )
	{
		pRegisteredTrack = m_lstRegisteredTracks.RemoveHead();
		delete pRegisteredTrack;
	}

 	RegisteredObject* pRegisteredObject;
	while( !m_lstRegisteredObjects.IsEmpty() )
	{
		pRegisteredObject = static_cast<RegisteredObject*>( m_lstRegisteredObjects.RemoveHead() );
		delete pRegisteredObject;
	}

	ASSERT( m_lstEmbeddedFiles.IsEmpty() );
 	EmbeddedFile* pEmbeddedFile;
	while( !m_lstEmbeddedFiles.IsEmpty() )
	{
		pEmbeddedFile = static_cast<EmbeddedFile*>( m_lstEmbeddedFiles.RemoveHead() );
		delete pEmbeddedFile;
	}
	m_dwEmbeddedFileListUseCount = 0;
	RELEASE( m_pIEmbeddedFileStream );

	if( m_pEmbeddedFileRootFile )
	{
		delete m_pEmbeddedFileRootFile;
		m_pEmbeddedFileRootFile = NULL;
	}

	RELEASE( m_pIToolGraphComponent );
	RELEASE( m_pIAudioPathComponent );
	RELEASE( m_pIContainerComponent );
	RELEASE( m_pISegmentComponent );
	RELEASE( m_pIFramework8 );
	RELEASE( m_pISongDocType8 );
	RELEASE( m_pIDMPerformance8 );
	RELEASE( m_pIConductor );
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent::LoadRegisteredObjects

BOOL CSongComponent::LoadRegisteredObjects( void )
{
	HKEY	  hKeyOpen;
	HKEY	  hKeyOpenClsId;
	DWORD	  dwIndex;
	LONG	  lResult;
	DWORD	  dwType;
	DWORD	  dwCbData;
	FILETIME  ftFileTime;
	wchar_t	  awchBuffer[MID_BUFFER << 1];
	_TCHAR	  achClsId[MID_BUFFER];
	_TCHAR	  achGuid[MID_BUFFER];
	RegisteredObject* pRegisteredObject;

	lResult = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE,
						  	  _T("Software\\Microsoft\\DMUSProducer\\Container Objects"),
							  0, KEY_READ, &hKeyOpen );
	if( lResult != ERROR_SUCCESS )
	{
		AfxMessageBox( IDS_ERR_REG_OBJECT);
		return FALSE;
	}

	dwIndex = 0;
	for( ; ; )
	{
		dwCbData = MID_BUFFER;
		lResult  = ::RegEnumKeyEx( hKeyOpen, dwIndex++, achClsId, &dwCbData,
								   NULL, NULL, NULL, &ftFileTime );
		if( lResult == ERROR_NO_MORE_ITEMS )
		{
			break;
		}
		if( lResult != ERROR_SUCCESS )
		{
			AfxMessageBox( IDS_ERR_REG_OBJECT);
			break;
		}

		lResult    = ::RegOpenKeyEx( hKeyOpen, achClsId,
								     0, KEY_QUERY_VALUE, &hKeyOpenClsId );
		if( lResult == ERROR_SUCCESS )
		{
			// We have another registered object
			pRegisteredObject = new RegisteredObject();
			
			if( pRegisteredObject )
			{
				// Store GUID of Node
				if( MultiByteToWideChar( CP_ACP, 0, achClsId, -1, awchBuffer, sizeof(awchBuffer) / sizeof(wchar_t) ) != 0 )
				{
					IIDFromString( awchBuffer, &pRegisteredObject->guidNodeId );
				}

				// Store object type of Node
				dwCbData = MID_BUFFER;
				lResult  = ::RegQueryValueEx( hKeyOpenClsId, _T(""), NULL,
											  &dwType, (LPBYTE)&achGuid, &dwCbData );
				if( (lResult == ERROR_SUCCESS)
				&&  (dwType == REG_SZ) )
				{
					pRegisteredObject->strObjectType = achGuid;
				}

				// Store GUID of RefNode
				dwCbData = MID_BUFFER;
				lResult  = ::RegQueryValueEx( hKeyOpenClsId, _T("RefNode"), NULL,
											  &dwType, (LPBYTE)&achGuid, &dwCbData );
				if( (lResult == ERROR_SUCCESS)
				&&  (dwType == REG_SZ) )
				{
					if( MultiByteToWideChar( CP_ACP, 0, achGuid, -1, awchBuffer, sizeof(awchBuffer) / sizeof(wchar_t) ) != 0 )
					{
						IIDFromString( awchBuffer, &pRegisteredObject->guidRefNodeId );
					}
				}
			
				// Store CLSID of Component
				dwCbData = MID_BUFFER;
				lResult  = ::RegQueryValueEx( hKeyOpenClsId, _T("Component"), NULL,
											  &dwType, (LPBYTE)&achGuid, &dwCbData );
				if( (lResult == ERROR_SUCCESS)
				&&  (dwType == REG_SZ) )
				{
					if( MultiByteToWideChar( CP_ACP, 0, achGuid, -1, awchBuffer, sizeof(awchBuffer) / sizeof(wchar_t) ) != 0 )
					{
						IIDFromString( awchBuffer, &pRegisteredObject->clsidComponent );
					}
				}
			
				// Store CLSID of corresponding DirectMusic object
				dwCbData = MID_BUFFER;
				lResult  = ::RegQueryValueEx( hKeyOpenClsId, _T("DMObject"), NULL,
											  &dwType, (LPBYTE)&achGuid, &dwCbData );
				if( (lResult == ERROR_SUCCESS)
				&&  (dwType == REG_SZ) )
				{
					if( MultiByteToWideChar( CP_ACP, 0, achGuid, -1, awchBuffer, sizeof(awchBuffer) / sizeof(wchar_t) ) != 0 )
					{
						IIDFromString( awchBuffer, &pRegisteredObject->clsidDMObject );
					}
				}

				// Place registered object in list
				m_lstRegisteredObjects.AddTail( pRegisteredObject );
			}
			
			::RegCloseKey( hKeyOpenClsId );
		}
	}

	::RegCloseKey( hKeyOpen );

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent::FindRegisteredObjectByCLSID

RegisteredObject* CSongComponent::FindRegisteredObjectByCLSID( CLSID clsidDMObject )
{
	RegisteredObject* pTheRegisteredObject = NULL;

	RegisteredObject* pRegisteredObject;
	POSITION pos = m_lstRegisteredObjects.GetHeadPosition();
	while( pos )
	{
		pRegisteredObject = m_lstRegisteredObjects.GetNext( pos );

		if( ::IsEqualGUID( pRegisteredObject->clsidDMObject, clsidDMObject ) )
		{
			pTheRegisteredObject = pRegisteredObject;
			break;
		}
	}

	return pTheRegisteredObject;
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent::FindRegisteredObjectByRIFFIds

RegisteredObject* CSongComponent::FindRegisteredObjectByRIFFIds( DWORD dwRIFFckid, DWORD dwRIFFfccType )
{
	RegisteredObject* pTheRegisteredObject = NULL;

	RegisteredObject* pRegisteredObject;
	POSITION pos = m_lstRegisteredObjects.GetHeadPosition();
	while( pos )
	{
		pRegisteredObject = m_lstRegisteredObjects.GetNext( pos );

		if( pRegisteredObject->dwRIFFckid == 0
		&&  pRegisteredObject->dwRIFFfccType == 0 )
		{
			// Needs to be initialized!
			// Can't initialize in LoadRegisteredObjects() because
			// Components still in process of loading and all DocTypes
			// have not been registered
			IDMUSProdDocType* pIDocType;
			if( SUCCEEDED ( m_pIFramework8->FindDocTypeByNodeId( pRegisteredObject->guidNodeId, &pIDocType ) ) )
			{
				IDMUSProdDocType8* pIDocType8;
				if( SUCCEEDED ( pIDocType->QueryInterface( IID_IDMUSProdDocType8, (void**)&pIDocType8 ) ) )
				{
					DWORD dwRIFFckid;
					DWORD dwRIFFfccType;
					if( SUCCEEDED ( pIDocType8->GetObjectRiffId( pRegisteredObject->guidNodeId, &dwRIFFckid, &dwRIFFfccType ) ) )
					{
						pRegisteredObject->dwRIFFckid = dwRIFFckid;
						pRegisteredObject->dwRIFFfccType = dwRIFFfccType;
					}
			
					RELEASE( pIDocType8 );
				}

				RELEASE( pIDocType );
			}
		}

		if( pRegisteredObject->dwRIFFckid == dwRIFFckid
		&&  pRegisteredObject->dwRIFFfccType == dwRIFFfccType )
		{
			pTheRegisteredObject = pRegisteredObject;
			break;
		}
	}

	return pTheRegisteredObject;
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent IUnknown implementation

HRESULT CSongComponent::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdComponent)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDMUSProdComponent *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IDMUSProdRIFFExt) )
    {
        AddRef();
        *ppvObj = (IDMUSProdRIFFExt *)this;
        return S_OK;
    }
	
    if( ::IsEqualIID(riid, IID_IDMUSProdUnpackingFiles) )
    {
        AddRef();
        *ppvObj = (IDMUSProdUnpackingFiles *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CSongComponent::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CSongComponent::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( m_dwRef != 0 );

	AfxOleUnlockApp(); 
    --m_dwRef;

    if( m_dwRef == 0 )
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent IDMUSProdComponent implementation

/////////////////////////////////////////////////////////////////////////////
// CSongComponent IDMUSProdComponent::Initialize

HRESULT CSongComponent::Initialize( IDMUSProdFramework* pIFramework, BSTR* pbstrErrMsg )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	TCHAR achErrMsg[MID_BUFFER];
	CString strErrMsg;

	if( m_pIFramework8 )		// already initialized
	{
		return S_OK;
	}

	ASSERT( pIFramework != NULL );
	ASSERT( pbstrErrMsg != NULL );

	if( pbstrErrMsg == NULL )
	{
		return E_POINTER;
	}

	if( pIFramework == NULL )
	{
		::LoadString( theApp.m_hInstance, IDS_ERR_INVALIDARG, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_INVALIDARG;
	}

	// Get IDMUSProdFramework8 interface pointers
	if( FAILED ( pIFramework->QueryInterface( IID_IDMUSProdFramework8, (void**)&m_pIFramework8 ) ) )
	{
		m_pIFramework8 = NULL;
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_INVALIDARG, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	theApp.m_pSongComponent = this;
//	theApp.m_pSongComponent->AddRef();	intentionally missing

	// Get IConductor and IDirectMusicPerformance interface pointers 
	IDMUSProdComponent* pIComponent = NULL;
	IUnknown* pIUnknown = NULL;
	if( FAILED ( pIFramework->FindComponent( CLSID_CConductor,  &pIComponent ) )
	||  FAILED ( pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&m_pIConductor ) )
	||  FAILED ( m_pIConductor->GetPerformanceEngine( (IUnknown**)&pIUnknown ) ) 
	||  FAILED ( pIUnknown->QueryInterface( IID_IDirectMusicPerformance8, (void**)&m_pIDMPerformance8 ) ) )
	{
		ReleaseAll();
		RELEASE( pIComponent );
		RELEASE( pIUnknown );
		::LoadString( theApp.m_hInstance, IDS_ERR_MISSING_CONDUCTOR, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}
	RELEASE( pIComponent );
	RELEASE( pIUnknown );

	// Get IDMUSProdComponent for Segment Designer
	if( FAILED ( pIFramework->FindComponent( CLSID_SegmentComponent, &m_pISegmentComponent ) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_MISSING_SEGMENT_DESIGNER, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Get IDMUSProdComponent for Container Designer
	if( FAILED ( pIFramework->FindComponent( CLSID_ContainerComponent, &m_pIContainerComponent ) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_MISSING_CONTAINER_DESIGNER, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Get IDMUSProdComponent for AudioPath Designer
	if( FAILED ( pIFramework->FindComponent( CLSID_AudioPathComponent, &m_pIAudioPathComponent ) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_MISSING_AUDIOPATH_DESIGNER, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Get IDMUSProdComponent for ToolGraph Designer
	if( FAILED ( pIFramework->FindComponent( CLSID_ToolGraphComponent, &m_pIToolGraphComponent ) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_MISSING_TOOLGRAPH_DESIGNER, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Add applicable images to the Project Tree control's image list 
	if( FAILED ( AddNodeImageLists() ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_IMAGELIST, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Register clipboard formats
	if( RegisterClipboardFormats() == FALSE )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_REGISTER_CF, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Register the clipboard format for an .snp file 
	CString strExt = _T(".snp");
	BSTR bstrExt = strExt.AllocSysString();
	if( FAILED ( pIFramework->RegisterClipFormatForFile(m_cfSong, bstrExt) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_CLIPFORMAT, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Register applicable doc types with the Framework 
	m_pISongDocType8 = new CSongDocType;
    if( m_pISongDocType8 == NULL )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_MEMORY, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_OUTOFMEMORY;
	}

	m_pISongDocType8->AddRef();

	if( FAILED ( pIFramework->AddDocType(m_pISongDocType8) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_DOCTYPE, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Load registered tracks
	LoadRegisteredTracks();

	// Load registered objects
	LoadRegisteredObjects();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent IDMUSProdComponent::CleanUp

HRESULT CSongComponent::CleanUp( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Release references to Songs
	CDirectMusicSong *pSong;
	while( !m_lstSongs.IsEmpty() )
	{
		pSong = static_cast<CDirectMusicSong*>( m_lstSongs.RemoveHead() );
		RELEASE( pSong );
	}
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent IDMUSProdComponent::GetName

HRESULT CSongComponent::GetName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;
	TCHAR achBuffer[MID_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDS_SONG_COMPONENT_NAME, achBuffer, MID_BUFFER) )
	{
		strName = achBuffer;
	}

    *pbstrName = strName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent::IDMUSProdComponent::AllocReferenceNode

HRESULT CSongComponent::AllocReferenceNode( GUID guidRefNodeId, IDMUSProdNode** ppIRefNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIRefNode == NULL )
	{
		return E_POINTER;
	}

	*ppIRefNode = NULL;

	// Make sure Component can create Nodes of type guidRefNodeId
	if( !( IsEqualGUID ( guidRefNodeId, GUID_SongRefNode ) ) )
	{
		return E_INVALIDARG;
	}

	// Create SongRefNode
	CSongRef* pSongRef = new CSongRef;
	if( pSongRef == NULL )
	{
		return E_OUTOFMEMORY ;
	}

	*ppIRefNode = (IDMUSProdNode *)pSongRef;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent IDMUSProdComponent::OnActivateApp

HRESULT CSongComponent::OnActivateApp( BOOL fActivate )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fActivate);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent IDMUSProdRIFFExt::LoadRIFFChunk

HRESULT CSongComponent::LoadRIFFChunk( IStream* pIStream, IDMUSProdNode** ppINode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDirectMusicSong* pSong;
	HRESULT hr;

	ASSERT( pIStream != NULL );
	ASSERT( m_pIFramework8 != NULL );

	if( ppINode == NULL )
	{
		return E_POINTER;
	}

	*ppINode = NULL;

	// Create a new Song 
	pSong = new CDirectMusicSong;
	if( pSong == NULL )
	{
		return E_OUTOFMEMORY ;
	}

	// Create the Undo Manager
	if( pSong->CreateUndoMgr() == FALSE )
	{
		pSong->Release();
		return E_OUTOFMEMORY;
	}

	// Load Song file
	hr = pSong->Load( pIStream );
	if( FAILED ( hr ) )
	{
		pSong->Release();
		return hr;
	}

	*ppINode = (IDMUSProdNode *)pSong;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent IDMUSProdUnpackingFiles implementation

/////////////////////////////////////////////////////////////////////////////
// CSongComponent IDMUSProdUnpackingFiles::GetDocRootOfEmbeddedFile

HRESULT CSongComponent::GetDocRootOfEmbeddedFile( IUnknown* pIDocType, BSTR bstrObjectName, IUnknown** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIFramework8 != NULL );

	CString strObjectName;
	if( bstrObjectName == NULL )
	{
		return E_INVALIDARG;
	}
	else
	{
		strObjectName = bstrObjectName;
		::SysFreeString( bstrObjectName );
	}

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}
	*ppIDocRootNode = NULL;

	if( pIDocType == NULL )
	{
		return E_INVALIDARG;
	}

	HRESULT hr = E_FAIL;

	POSITION pos = m_lstEmbeddedFiles.GetHeadPosition();
	while( pos )
	{
		EmbeddedFile* pEmbeddedFile = m_lstEmbeddedFiles.GetNext( pos );

		ASSERT( pEmbeddedFile->dwStreamPos != 0 );	// temporary check

		if( strObjectName.CompareNoCase( pEmbeddedFile->strObjectName ) == 0 )
		{
			// Object names are equal
			RegisteredObject* pRegisteredObject = FindRegisteredObjectByCLSID( pEmbeddedFile->guidDMClass );
			if( pRegisteredObject )
			{
				// Check to see if CLSID is type of object we want
				IDMUSProdDocType* pIDocTypeList;
				if( SUCCEEDED ( m_pIFramework8->FindDocTypeByNodeId( pRegisteredObject->guidNodeId, &pIDocTypeList ) ) )
				{
					if( pIDocTypeList == pIDocType )
					{
						// This is the object we are looking for...
						if( pEmbeddedFile->fBeingLoaded )
						{
							// We are already in the process of loading this file
							hr = E_PENDING;
						}
						else
						{
							// Set m_pIEmbeddedFileStream position
							DWORD dwOrigStreamPos = StreamTell( m_pIEmbeddedFileStream );
							StreamSeek( m_pIEmbeddedFileStream, pEmbeddedFile->dwStreamPos, 0 );

							*ppIDocRootNode = LoadEmbeddedSegment( m_pIEmbeddedFileStream );
							if( *ppIDocRootNode )
							{
								hr = S_OK;
							}

							// Restore original m_pIEmbeddedFileStream position
							StreamSeek( m_pIEmbeddedFileStream, dwOrigStreamPos, 0 );
						}

						RELEASE( pIDocTypeList );
						break;
					}
				
					RELEASE( pIDocTypeList );
				}
			}
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent IDMUSProdUnpackingFiles::AddToNotifyWhenLoadFinished

HRESULT CSongComponent::AddToNotifyWhenLoadFinished( IUnknown* pIDocType, BSTR bstrObjectName, IUnknown* punkNotifySink, GUID* pguidFile )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIFramework8 != NULL );

	if( pguidFile )
	{
		*pguidFile = GUID_AllZeros;
	}

	CString strObjectName;
	if( bstrObjectName == NULL )
	{
		return E_INVALIDARG;
	}
	else
	{
		strObjectName = bstrObjectName;
		::SysFreeString( bstrObjectName );
	}

	if( pguidFile == NULL )
	{
		return E_POINTER;
	}

	if( pIDocType == NULL 
	||  punkNotifySink == NULL )
	{
		return E_INVALIDARG;
	}

	HRESULT hr = E_FAIL;

	POSITION pos = m_lstEmbeddedFiles.GetHeadPosition();
	while( pos )
	{
		EmbeddedFile* pEmbeddedFile = m_lstEmbeddedFiles.GetNext( pos );

		ASSERT( pEmbeddedFile->dwStreamPos != 0 );	// temporary check

		if( strObjectName.CompareNoCase( pEmbeddedFile->strObjectName ) == 0 )
		{
			// Object names are equal
			RegisteredObject* pRegisteredObject = FindRegisteredObjectByCLSID( pEmbeddedFile->guidDMClass );
			if( pRegisteredObject )
			{
				// Check to see if CLSID is type of object we want
				IDMUSProdDocType* pIDocTypeList;
				if( SUCCEEDED ( m_pIFramework8->FindDocTypeByNodeId( pRegisteredObject->guidNodeId, &pIDocTypeList ) ) )
				{
					if( pIDocTypeList == pIDocType )
					{
						// This is the object we are looking for...
						if( pEmbeddedFile->fBeingLoaded )
						{
							// Add pINotifySink to embedded file's notification list
							IDMUSProdNotifySink* pINotifySink;
							if( SUCCEEDED ( punkNotifySink->QueryInterface( IID_IDMUSProdNotifySink, (void **)&pINotifySink ) ) )
							{
								pEmbeddedFile->m_lstNotifyWhenLoadFinished.AddTail( pINotifySink );
								*pguidFile = pEmbeddedFile->guidNotification;
								hr = S_OK;
							}
						}

						RELEASE( pIDocTypeList );
						break;
					}
				
					RELEASE( pIDocTypeList );
				}
			}
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent additional functions

/////////////////////////////////////////////////////////////////////////////
// CSongComponent::AddNodeImageLists

HRESULT CSongComponent::AddNodeImageLists( void )
{
	CImageList lstImages;
	HICON hIcon;

	lstImages.Create( 16, 16, ILC_COLOR16, 4, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDR_SONG_DOCTYPE) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_SONG_DOCTYPE_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_SONGREF) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_SONGREF_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_FOLDER) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_FOLDER_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( FAILED (m_pIFramework8->AddNodeImageList( lstImages.Detach(), &m_nFirstImage ) ) )
	{
		return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent::RegisterClipboardFormats

BOOL CSongComponent::RegisterClipboardFormats( void )
{
	m_cfProducerFile = ::RegisterClipboardFormat( CF_DMUSPROD_FILE );
	m_cfSong = ::RegisterClipboardFormat( CF_SONG );
	m_cfSongList = ::RegisterClipboardFormat( CF_SONGLIST );
	m_cfSegment = ::RegisterClipboardFormat( CF_SEGMENT );
	m_cfContainer = ::RegisterClipboardFormat( CF_CONTAINER );
	m_cfAudioPath = ::RegisterClipboardFormat( CF_AUDIOPATH );
	m_cfGraph = ::RegisterClipboardFormat( CF_GRAPH );
	m_cfVirtualSegmentList = ::RegisterClipboardFormat( CF_VIRTUAL_SEGMENT_LIST );
	m_cfTrackList = ::RegisterClipboardFormat( CF_TRACK_LIST );
	m_cfTransitionList = ::RegisterClipboardFormat( CF_TRANSITION_LIST );

	if( m_cfProducerFile == 0
	||  m_cfSong == 0
	||  m_cfSongList == 0 
	||  m_cfSegment == 0
	||  m_cfContainer == 0
	||  m_cfAudioPath == 0
	||  m_cfGraph == 0
	||  m_cfVirtualSegmentList == 0
	||  m_cfTrackList == 0
	||  m_cfTransitionList == 0 )
	{
		return FALSE;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent::LoadRegisteredTracks

HRESULT CSongComponent::LoadRegisteredTracks( void )
{
	HKEY	  hKeyOpen;
	HKEY	  hKeyOpenClsId;
	DWORD	  dwType;
	DWORD	  dwCbData;
	FILETIME  ftFileTime;
	_TCHAR	  achClsId[MID_BUFFER];
	_TCHAR	  achName[MID_BUFFER];

	LONG lResult = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE,
							  	   _T("Software\\Microsoft\\DMUSProducer\\StripEditors"),
								   0, KEY_READ, &hKeyOpen );
	if( lResult != ERROR_SUCCESS )
	{
		return S_FALSE;
	}

	HRESULT hr = S_FALSE;

	DWORD dwIndex = 0;
	for( ; ; )
	{
		dwCbData = MID_BUFFER;
		lResult  = ::RegEnumKeyEx( hKeyOpen, dwIndex++, achClsId, &dwCbData,
								   NULL, NULL, NULL, &ftFileTime );
		if( lResult != ERROR_SUCCESS )
		{
			break;
		}

		_tcscpy( achName, _T("") );
		lResult    = ::RegOpenKeyEx( hKeyOpen, achClsId,
								     0, KEY_QUERY_VALUE, &hKeyOpenClsId );
		if( lResult == ERROR_SUCCESS )
		{
			dwCbData = MID_BUFFER;
			lResult  = ::RegQueryValueEx( hKeyOpenClsId, _T(""), NULL,
										  &dwType, (LPBYTE)&achName, &dwCbData );
			if( (lResult == ERROR_SUCCESS)
			&&  (dwType == REG_SZ) )
			{
				CLSID clsid;
				wchar_t awchClsId[80];

				if( MultiByteToWideChar( CP_ACP, 0, achClsId, -1, awchClsId, sizeof(awchClsId) / sizeof(wchar_t) ) != 0 )
				{
					IIDFromString( awchClsId, &clsid );

					RegisteredTrack* pRegisteredTrack = new RegisteredTrack;
					if( pRegisteredTrack )
					{
						pRegisteredTrack->clsidTrack = clsid;
						pRegisteredTrack->strName = achName;

						m_lstRegisteredTracks.AddTail( pRegisteredTrack );
						hr = S_OK;
					}
				}
			}

			::RegCloseKey( hKeyOpenClsId );
		}
	}

	::RegCloseKey( hKeyOpen );

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent::GetRegisteredTrackName

void CSongComponent::GetRegisteredTrackName( CLSID clsidTrack, CString& strName )
{
	strName.Empty();

	POSITION pos = m_lstRegisteredTracks.GetHeadPosition();
	while( pos )
	{
		RegisteredTrack* pRegisteredTrack = m_lstRegisteredTracks.GetNext( pos );

		if( ::IsEqualGUID( pRegisteredTrack->clsidTrack, clsidTrack ) )
		{
			strName = pRegisteredTrack->strName;
			break;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent::GetSongImageIndex

HRESULT CSongComponent::GetSongImageIndex( short* pnFirstImage )
{
	*pnFirstImage = (short)(m_nFirstImage + FIRST_SONG_IMAGE);
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent::GetSongRefImageIndex

HRESULT CSongComponent::GetSongRefImageIndex( short* pnFirstImage )
{
	*pnFirstImage = (short)(m_nFirstImage + FIRST_SONGREF_IMAGE);
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent::GetFolderImageIndex

HRESULT CSongComponent::GetFolderImageIndex( short* pnFirstImage )
{
	*pnFirstImage = (short)(m_nFirstImage + FIRST_FOLDER_IMAGE);
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent::DetermineAction

int CSongComponent::DetermineAction( IDMUSProdNode* pIDocRootNode, IStream* pIStream, IDMUSProdNode** ppIDupeDocRootNode )
{
	int nReturn = IDC_KEEP_BOTH;

	IDMUSProdNode* pIDupeFileNode = NULL;

	if( ppIDupeDocRootNode == NULL )
	{
		return nReturn;
	}
	*ppIDupeDocRootNode = NULL;

	if( pIDocRootNode == NULL 
	||  pIStream == NULL )
	{
		return nReturn;
	}

	// Get guidNodeId of pIDocRootNode
	GUID guidNodeId;
	if( FAILED ( pIDocRootNode->GetNodeId( &guidNodeId ) ) )
	{
		return nReturn;
	}

	// Get DocRoot of pIDocRootNode
	IDMUSProdDocType* pIDocType = NULL;
	if( FAILED ( m_pIFramework8->FindDocTypeByNodeId( guidNodeId, &pIDocType ) ) )
	{
		return nReturn;
	}

	// Initialize the DMUS_OBJECTDESC structure
	DMUS_OBJECTDESC dmusObjectDesc;
	memset( &dmusObjectDesc, 0, sizeof(DMUS_OBJECTDESC) );
	dmusObjectDesc.dwSize = sizeof(DMUS_OBJECTDESC);

	CString strDupeFileNodeName;

	// Get object descriptor for pIDocRootNode
	IDMUSProdDocType8* pIDocType8;
	if( SUCCEEDED ( pIDocType->QueryInterface( IID_IDMUSProdDocType8, (void**)&pIDocType8 ) ) )
	{
		if( SUCCEEDED ( pIDocType8->GetObjectDescriptorFromNode( pIDocRootNode, &dmusObjectDesc ) ) )
		{
			IDMUSProdProject* pINextProject;
			IDMUSProdProject* pIProject;
			IDMUSProdNode* pINextFileNode;
			IDMUSProdNode* pIFileNode;
			DMUSProdListInfo ListInfo;

			// Search all files in the Project Tree for a matching object descriptor
			HRESULT hrProject = m_pIFramework8->GetFirstProject( &pINextProject );

			while( (pIDupeFileNode == NULL)  &&  SUCCEEDED( hrProject )  &&  pINextProject )
			{
				pIProject = pINextProject;

				HRESULT hrFile = pIProject->GetFirstFileByDocType( pIDocType, &pINextFileNode );

				while( (pIDupeFileNode == NULL)  &&  (hrFile == S_OK) )
				{
					pIFileNode = pINextFileNode;

					ZeroMemory( &ListInfo, sizeof(ListInfo) );
					ListInfo.wSize = sizeof(ListInfo);

					if( SUCCEEDED ( pIFileNode->GetNodeListInfo ( &ListInfo ) ) )
					{
						CString strName;
						if( ListInfo.bstrName )
						{
							strName = ListInfo.bstrName;
							::SysFreeString( ListInfo.bstrName );
						}
						if( ListInfo.bstrDescriptor )
						{
							::SysFreeString( ListInfo.bstrDescriptor );
						}

						if( (strName == dmusObjectDesc.wszName)
						&&  (::IsEqualGUID(ListInfo.guidObject, dmusObjectDesc.guidObject)) )
						{
							strDupeFileNodeName = strName;
							pIDupeFileNode = pIFileNode;
							pIDupeFileNode->AddRef();
						}
					}

					if( pIDupeFileNode == NULL )
					{
						hrFile = pIProject->GetNextFileByDocType( pIFileNode, &pINextFileNode );
					}
					RELEASE( pIFileNode );
				}
			
				if( pIDupeFileNode == NULL )
				{
					hrProject = m_pIFramework8->GetNextProject( pIProject, &pINextProject );
				}
				RELEASE( pIProject );
			}
		}

		RELEASE( pIDocType8 );
	}

	if( pIDupeFileNode )
	{
		CDupeFileDlg dlgDupeFile;

		// Get Project name
		CString strProjectName;
		IDMUSProdProject* pIProject;
		if( SUCCEEDED ( m_pIFramework8->FindProject( pIDupeFileNode, &pIProject ) ) )
		{
			BSTR bstrProjectName;
			if( SUCCEEDED ( pIProject->GetName( &bstrProjectName ) ) )
			{
				strProjectName = bstrProjectName;
				::SysFreeString( bstrProjectName );
			}

			RELEASE( pIProject );
		}

		// Get Container's filename
		CString strContainerFileName;
		IDMUSProdPersistInfo* pIPersistInfo;
		if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pIPersistInfo ) ) )
		{
			BSTR bstrContainerFileName;
			if( SUCCEEDED ( pIPersistInfo->GetFileName( &bstrContainerFileName ) ) )
			{
				strContainerFileName = bstrContainerFileName;
				::SysFreeString( bstrContainerFileName );
			}

			TCHAR achFileName[FILENAME_MAX];
			TCHAR achFName[_MAX_FNAME];
			TCHAR achExt[_MAX_EXT];

			_tsplitpath( strContainerFileName, NULL, NULL, achFName, achExt );
			_tmakepath( achFileName, NULL, NULL, achFName, achExt );
			strContainerFileName = achFileName;

			RELEASE( pIPersistInfo );
		}
		
		// Prepare dialog's prompt text
		CString strText;
		CString strSegment;
		strSegment.LoadString( IDS_SEGMENT_TEXT );
		strSegment.MakeLower();
		AfxFormatString2( dlgDupeFile.m_strPrompt, IDS_DUPE_FILE_PROMPT1, strProjectName, strSegment );
		AfxFormatString2( strText, IDS_DUPE_FILE_PROMPT2, strDupeFileNodeName, strContainerFileName );
		dlgDupeFile.m_strPrompt += strText;

		// Prepare dialog's 'Use Existing' prompt text
		AfxFormatString1( dlgDupeFile.m_strPromptUseExisting, IDS_DUPE_FILE_PROMPT3, strProjectName );

		// Prepare dialog's 'Use Embedded' prompt text
		AfxFormatString1( dlgDupeFile.m_strPromptUseEmbedded, IDS_DUPE_FILE_PROMPT4, strProjectName );

		// Prepare dialog's 'Keep Both' prompt text
		if( m_pIDupeFileTargetProject )
		{
			CString strTargetProjectName;
			BSTR bstrTargetProjectName;
			if( SUCCEEDED ( m_pIDupeFileTargetProject->GetName( &bstrTargetProjectName ) ) )
			{
				strTargetProjectName = bstrTargetProjectName;
				::SysFreeString( bstrTargetProjectName );
			}
			AfxFormatString1( dlgDupeFile.m_strPromptKeepBoth, IDS_DUPE_FILE_PROMPT5, strTargetProjectName );
		}
		else
		{
			dlgDupeFile.m_strPromptKeepBoth.LoadString( IDS_DUPE_FILE_PROMPT6 );
		}

		// Ask user what they want to do
		switch( m_nDupeFileDlgReturnCode )
		{
			case IDC_USE_EXISTING_ALL_FILES:
			case IDC_USE_EMBEDDED_ALL_FILES:
			case IDC_KEEP_BOTH_ALL_FILES:
				nReturn = m_nDupeFileDlgReturnCode;
				break;

			default:
				nReturn = dlgDupeFile.DoModal();
				if( nReturn == IDC_USE_EXISTING_ALL_FILES
				||  nReturn == IDC_USE_EMBEDDED_ALL_FILES
				||  nReturn == IDC_KEEP_BOTH_ALL_FILES )
				{
					m_nDupeFileDlgReturnCode = nReturn;
				}
				break;
		}

		switch( nReturn )
		{
			case IDC_USE_EXISTING:
			case IDC_USE_EXISTING_ALL_FILES:
			case IDC_USE_EMBEDDED:
			case IDC_USE_EMBEDDED_ALL_FILES:
				// Get the FileNode's DocRoot node
				GUID guidFile;
				if( SUCCEEDED ( m_pIFramework8->GetNodeFileGUID ( pIDupeFileNode, &guidFile ) ) )
				{
					IDMUSProdNode* pIDupeDocRootNode;
					if( SUCCEEDED ( m_pIFramework8->FindDocRootNodeByFileGUID(guidFile, &pIDupeDocRootNode) ) )
					{
						*ppIDupeDocRootNode = pIDupeDocRootNode;
					}
				}
				break;
			
			case IDC_KEEP_BOTH:
			case IDC_KEEP_BOTH_ALL_FILES:
				// Don't need to open the file to get the FileNode's DocRoot node
				break;

			default:
				// Should not happen!
				ASSERT( 0 );
				break;
		}
	}
		
	RELEASE( pIDupeFileNode );
	RELEASE( pIDocType );
	return nReturn;
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent::LoadEmbeddedSegment

IDMUSProdNode* CSongComponent::LoadEmbeddedSegment( IStream* pIStream )
{
	IDMUSProdNode*		pITheDocRootNode = NULL;
	IDMUSProdNode*		pITargetDirectoryNode = NULL;
	IDMUSProdNode*		pIDocRootNode;
	IDMUSProdNode*		pIDupeDocRootNode;
	IDMUSProdNode*		pINewDocRootNode;
	HRESULT				hr;

	ASSERT( m_pISegmentComponent != NULL );
	ASSERT( m_pIFramework8 != NULL );
	ASSERT( pIStream != NULL );

	// No need to load the segment again if it is already loaded
	EmbeddedFile* pTheEmbeddedFile = NULL;
	DWORD dwStreamPos = StreamTell( pIStream );
	POSITION pos = m_lstEmbeddedFiles.GetHeadPosition();
	while( pos )
	{
		EmbeddedFile* pEmbeddedFile = m_lstEmbeddedFiles.GetNext( pos );

		if( pEmbeddedFile->dwStreamPos == dwStreamPos )
		{
			pTheEmbeddedFile = pEmbeddedFile;

			if( ::IsEqualGUID(pEmbeddedFile->guidFile, GUID_AllZeros) == FALSE )
			{
				if( SUCCEEDED ( m_pIFramework8->FindDocRootNodeByFileGUID( pEmbeddedFile->guidFile, &pIDocRootNode ) ) )
				{
					return pIDocRootNode;
				}
			}
			break;
		}
	}
	ASSERT( pTheEmbeddedFile );	// Just to make sure the EmbeddedFile list was built correctly

	// Set flag to indicate we are in the process of loading this embedded file
	if( pTheEmbeddedFile )
	{
		pTheEmbeddedFile->fBeingLoaded = true;
	}

	IDMUSProdRIFFExt* pIRIFFExt;
	if( SUCCEEDED ( m_pISegmentComponent->QueryInterface( IID_IDMUSProdRIFFExt, (void**)&pIRIFFExt ) ) )
	{
		hr = pIRIFFExt->LoadRIFFChunk( pIStream, &pIDocRootNode );
		if( SUCCEEDED ( hr ) )
		{
			// Set root and parent node of ALL children
			theApp.SetNodePointers( pIDocRootNode, pIDocRootNode, NULL );

			int nReturn = DetermineAction( pIDocRootNode, pIStream, &pIDupeDocRootNode );

			switch( nReturn )
			{
				case IDC_USE_EXISTING:
				case IDC_USE_EXISTING_ALL_FILES:
					// Use the file in the Project Tree
					pITheDocRootNode = pIDupeDocRootNode;
					break;

				case IDC_USE_EMBEDDED:
				case IDC_USE_EMBEDDED_ALL_FILES:
				case IDC_KEEP_BOTH:
				case IDC_KEEP_BOTH_ALL_FILES:
				{
					// Get the target directory
					DMUSProdStreamInfo	StreamInfo;
					StreamInfo.pITargetDirectoryNode = NULL;

					// Get additional stream information
					IDMUSProdPersistInfo* pPersistInfo;
					if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
					{
						pPersistInfo->GetStreamInfo( &StreamInfo );
						pITargetDirectoryNode = StreamInfo.pITargetDirectoryNode;
						pPersistInfo->Release();
					}

					// Place embedded file in the Project Tree
					if( SUCCEEDED ( m_pIFramework8->CopyFile( pIDocRootNode, pITargetDirectoryNode, &pINewDocRootNode ) ) )
					{
						// Use the embedded file that was just placed in the Project Tree
						pITheDocRootNode = pINewDocRootNode;

						if( nReturn == IDC_USE_EMBEDDED 
						||  nReturn == IDC_USE_EMBEDDED_ALL_FILES )
						{
							if( pIDupeDocRootNode )
							{
								// Update all references to point to the new file
								m_pIFramework8->NotifyNodes( pIDupeDocRootNode, FRAMEWORK_FileReplaced, pINewDocRootNode );
								// Delete the duplicate file
								pIDupeDocRootNode->DeleteNode( FALSE );
							}
						}
					}
					RELEASE( pIDupeDocRootNode );
					break;
				}

				default:
					// Should not happen
					ASSERT( 0 );
					break;
			}

			pIDocRootNode->DeleteNode( FALSE );
			RELEASE( pIDocRootNode );
		}

		RELEASE( pIRIFFExt );
	}

	if( pTheEmbeddedFile )
	{
		pTheEmbeddedFile->fBeingLoaded = false;
	    
		if( pITheDocRootNode )
		{
			m_pIFramework8->GetNodeFileGUID( pITheDocRootNode, &pTheEmbeddedFile->guidFile );

			// Send FRAMEWORK_FileLoadFinished notifications
			while( !pTheEmbeddedFile->m_lstNotifyWhenLoadFinished.IsEmpty() )
			{
				IDMUSProdNotifySink* pINotifySink = pTheEmbeddedFile->m_lstNotifyWhenLoadFinished.RemoveHead();

				pINotifySink->OnUpdate( pITheDocRootNode, FRAMEWORK_FileLoadFinished, &pTheEmbeddedFile->guidNotification );  
				RELEASE( pINotifySink );
			}
		}
	}

	return pITheDocRootNode;
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent::AddToSongFileList

void CSongComponent::AddToSongFileList( CDirectMusicSong* pSong )
{
	if( pSong )
	{
		GUID guidSong;
		pSong->GetGUID( &guidSong );

		// Prevent duplicate object GUIDs
		GUID guidSongList;
		POSITION pos = m_lstSongs.GetHeadPosition();
		while( pos )
		{
			CDirectMusicSong* pSongList = m_lstSongs.GetNext( pos );

			pSongList->GetGUID( &guidSongList );
			if( ::IsEqualGUID( guidSongList, guidSong ) )
			{
				::CoCreateGuid( &guidSong );
				pSong->SetGUID( guidSong );
				break;
			}
		}

		// Add to list
		pSong->AddRef();
		m_lstSongs.AddTail( pSong );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent::RemoveFromSongFileList

void CSongComponent::RemoveFromSongFileList( CDirectMusicSong* pSong )
{
	if( pSong )
	{
		// Remove from list
		POSITION pos = m_lstSongs.Find( pSong );
		if( pos )
		{
			m_lstSongs.RemoveAt( pos );
			pSong->Release();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent::CreateEmbeddedFileList

HRESULT CSongComponent::CreateEmbeddedFileList( IStream* pIStream )
{
	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	m_dwEmbeddedFileListUseCount++;

	if( m_dwEmbeddedFileListUseCount > 1 )
	{
		return S_OK;
	}

	m_pIEmbeddedFileStream = pIStream;
	m_pIEmbeddedFileStream->AddRef();

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;
    HRESULT hr = S_OK;

	if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		// Make sure the file containing this Container is stored in m_pEmbeddedFileRootFile
		DWORD dwCurrentFilePos = StreamTell( pIStream );
		if( dwCurrentFilePos > 0 )
		{
			MMCKINFO ckThisFile;

			StreamSeek( pIStream, 0, 0 );
			if( pIRiffStream->Descend( &ckThisFile, 0, 0 ) == 0 )
			{
				RegisteredObject* pRegisteredObject = FindRegisteredObjectByRIFFIds( ckThisFile.ckid, ckThisFile.fccType );
				if( pRegisteredObject )
				{
					IDMUSProdDocType* pIDocType;
					if( SUCCEEDED ( m_pIFramework8->FindDocTypeByNodeId( pRegisteredObject->guidNodeId, &pIDocType ) ) )
					{
						DMUSProdListInfo ListInfo;
						ZeroMemory( &ListInfo, sizeof(ListInfo) );
						ListInfo.wSize = sizeof(ListInfo);

						StreamSeek( pIStream, 0, 0 );
						if( SUCCEEDED ( pIDocType->GetListInfo ( pIStream, &ListInfo ) ) )
						{
							EmbeddedFile* pEmbeddedFile = new EmbeddedFile();
							if( pEmbeddedFile )
							{
								pEmbeddedFile->dwStreamPos = 0;
								pEmbeddedFile->fBeingLoaded = true;
								pEmbeddedFile->guidDMClass = pRegisteredObject->clsidDMObject;

								if( ListInfo.bstrName )
								{
									pEmbeddedFile->strObjectName = ListInfo.bstrName;
									::SysFreeString( ListInfo.bstrName );
								}
								if( ListInfo.bstrDescriptor )
								{
									::SysFreeString( ListInfo.bstrDescriptor );
								}
								memcpy( &pEmbeddedFile->guidObject, &ListInfo.guidObject, sizeof(GUID) );

								if( m_pEmbeddedFileRootFile )
								{
									// Names are equal
									if( pEmbeddedFile->strObjectName.Compare(m_pEmbeddedFileRootFile->strObjectName) == 0 )
									{
										// GUIDs are equal
										if( ::IsEqualGUID(pEmbeddedFile->guidDMClass, m_pEmbeddedFileRootFile->guidDMClass)
										&&  ::IsEqualGUID(pEmbeddedFile->guidObject, m_pEmbeddedFileRootFile->guidObject) )
										{
											// We must keep the notification GUID
											pEmbeddedFile->guidNotification = m_pEmbeddedFileRootFile->guidNotification;
										}
									}
									delete m_pEmbeddedFileRootFile;
									m_pEmbeddedFileRootFile = NULL;
								}
								m_pEmbeddedFileRootFile = pEmbeddedFile;
							}
						}

						RELEASE( pIDocType );
					}
				}
			}
			StreamSeek( pIStream, dwCurrentFilePos, 0 );
		}

		// Make sure all Source Segments embedded in this Song are placed in the EmbeddedFile list
		ckMain.fccType = DMUS_FOURCC_SONG_FORM;
		if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
		{
			hr = ParseSongForEmbeddedSegments( pIRiffStream, &ckMain );
		}

		RELEASE( pIRiffStream );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent::ReleaseEmbeddedFileList

void CSongComponent::ReleaseEmbeddedFileList( void )
{
    ASSERT( m_dwEmbeddedFileListUseCount != 0 );

    m_dwEmbeddedFileListUseCount--;

    if( m_dwEmbeddedFileListUseCount == 0 )
    {
 		EmbeddedFile* pEmbeddedFile;
		while( !m_lstEmbeddedFiles.IsEmpty() )
		{
			pEmbeddedFile = static_cast<EmbeddedFile*>( m_lstEmbeddedFiles.RemoveHead() );
			delete pEmbeddedFile;
		}

		RELEASE( m_pIEmbeddedFileStream );
    }
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent::ParseSongForEmbeddedSegments

HRESULT CSongComponent::ParseSongForEmbeddedSegments( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
    IStream*    pIStream;
	MMCKINFO	ck;
	MMCKINFO	ckList;
    HRESULT     hr = S_OK;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case FOURCC_RIFF:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_CONTAINER_FORM: 
						// Look for embedded Songs in this Song's Container
						hr = ParseContainerForEmbeddedSongs( pIRiffStream, &ck );
						if( FAILED ( hr ) )
						{
							goto ON_ERROR;
						}
						break;
				}
				break;

			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_SONGSEGMENTS_LIST:
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case FOURCC_LIST:
									switch( ckList.fccType )
									{
										case DMUS_FOURCC_SONGSEGMENT_LIST:
											hr = AddEmbeddedFileListItem( pIRiffStream, &ckList );
											if( FAILED ( hr ) )
											{
												ASSERT( 0 );
												goto ON_ERROR;
											}
											else if( hr == S_FALSE )
											{
												// File was referenced
												// Change S_FALSE to S_OK
												hr = S_OK;
											}
											break;
									}
							}
					        pIRiffStream->Ascend( &ckList, 0 );
						}
						break;
				}
				break;
        }

        pIRiffStream->Ascend( &ck, 0 );
    }

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent::ParseContainerForEmbeddedSongs

HRESULT CSongComponent::ParseContainerForEmbeddedSongs( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
    IStream*    pIStream;
	MMCKINFO	ck;
	MMCKINFO	ckList;
	MMCKINFO	ckObject;
    HRESULT     hr = S_OK;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_CONTAINED_OBJECTS_LIST:
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case FOURCC_LIST:
									switch( ckList.fccType )
									{
										case DMUS_FOURCC_CONTAINED_OBJECT_LIST:
											while( pIRiffStream->Descend( &ckObject, &ckList, 0 ) == 0 )
											{
												switch( ckObject.ckid )
												{
													case FOURCC_RIFF:
														switch( ckObject.fccType )
														{
															case DMUS_FOURCC_SONG_FORM:
																hr = ParseSongForEmbeddedSegments( pIRiffStream, &ckObject );
																if( FAILED ( hr ) )
																{
																	ASSERT( 0 );
																	goto ON_ERROR;
																}
																break;

															case DMUS_FOURCC_CONTAINER_FORM:
																hr = ParseContainerForEmbeddedSongs( pIRiffStream, &ckObject );
																if( FAILED ( hr ) )
																{
																	ASSERT( 0 );
																	goto ON_ERROR;
																}
																break;
														}
												}
										        pIRiffStream->Ascend( &ckObject, 0 );
											}
											break;
									}
							}
					        pIRiffStream->Ascend( &ckList, 0 );
						}
						break;
				}
				break;
        }

        pIRiffStream->Ascend( &ck, 0 );
    }

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent::AddEmbeddedFileListItem

HRESULT CSongComponent::AddEmbeddedFileListItem( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
    IStream*    pIStream;
	MMCKINFO	ck;
	DWORD		dwStartFilePos;
	DWORD		dwInsideFilePos;
    HRESULT     hr = S_OK;

	EmbeddedFile* pEmbeddedFile = NULL;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    dwStartFilePos = StreamTell( pIStream );

	// Find DMUS_FOURCC_REF_LIST - Is file embedded or referenced?
	ck.fccType = DMUS_FOURCC_REF_LIST;
	if( pIRiffStream->Descend( &ck, pckMain, MMIO_FINDLIST ) == 0 )
	{
		// File is referenced, nothing more to do
		hr = S_FALSE;
		goto ON_ERROR;
	}

	// Process embedded file
	{
		StreamSeek( pIStream, dwStartFilePos, 0 );

		MMCKINFO ckEmbeddedFile;
		ckEmbeddedFile.ckid = FOURCC_RIFF;
		ckEmbeddedFile.fccType = DMUS_FOURCC_SEGMENT_FORM;

		if( pIRiffStream->Descend( &ckEmbeddedFile, pckMain, MMIO_FINDRIFF ) == 0 )
		{
			dwInsideFilePos = StreamTell( pIStream );
			StreamSeek( pIStream, -12, STREAM_SEEK_CUR );
		}
		else
		{
			ASSERT( 0 );
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Fill in the EmbeddedFile struct
		if( SUCCEEDED ( hr ) )
		{
			hr = E_FAIL;

			pEmbeddedFile = new EmbeddedFile();
			if( pEmbeddedFile == NULL )
			{
				hr = E_OUTOFMEMORY;
				goto ON_ERROR;
			}

			pEmbeddedFile->dwStreamPos = StreamTell( pIStream );
			pEmbeddedFile->guidDMClass = CLSID_DirectMusicSegment;

			// Get DocType for GUID_SegmentNode
			IDMUSProdDocType* pIDocType;
			if( SUCCEEDED ( m_pIFramework8->FindDocTypeByNodeId( GUID_SegmentNode, &pIDocType ) ) )
			{
				DMUSProdListInfo ListInfo;
				ZeroMemory( &ListInfo, sizeof(ListInfo) );
				ListInfo.wSize = sizeof(ListInfo);

				if( SUCCEEDED ( pIDocType->GetListInfo ( pIStream, &ListInfo ) ) )
				{
					if( ListInfo.bstrName )
					{
						pEmbeddedFile->strObjectName = ListInfo.bstrName;
						::SysFreeString( ListInfo.bstrName );
					}
					if( ListInfo.bstrDescriptor )
					{
						::SysFreeString( ListInfo.bstrDescriptor );
					}
					memcpy( &pEmbeddedFile->guidObject, &ListInfo.guidObject, sizeof(GUID) );

					hr = S_OK;
				}

				RELEASE( pIDocType );
			}
		}
	}

ON_ERROR:
	if( hr == S_OK )
	{
		if( pEmbeddedFile )
		{
			// Keep track of the embedded Segment's stream position
			m_lstEmbeddedFiles.AddTail( pEmbeddedFile );

			// Look for a container in this embedded Segment
			MMCKINFO ckEmbeddedFile;
			StreamSeek( pIStream, dwInsideFilePos, 0 );
			ckEmbeddedFile.fccType = DMUS_FOURCC_CONTAINER_FORM;
			if( pIRiffStream->Descend( &ckEmbeddedFile, NULL, MMIO_FINDRIFF ) == 0 )
			{
				// Look for embedded Songs in this Segment's Container
				hr = ParseContainerForEmbeddedSongs( pIRiffStream, &ckEmbeddedFile );
			}
		}
		else
		{
			hr = E_UNEXPECTED;
		}
	}
	else
	{
		if( pEmbeddedFile )
		{
			delete pEmbeddedFile;
		}
	}
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSongComponent::FindEmbeddedFile

EmbeddedFile* CSongComponent::FindEmbeddedFile( IDMUSProdDocType* pIDocType, CString& strObjectName )
{
	ASSERT( m_pIFramework8 != NULL );

	EmbeddedFile* pTheEmbeddedFile = NULL;

	// This 'private' method must also search m_pEmbeddedFileRootFile
	if( m_pEmbeddedFileRootFile )
	{
		if( strObjectName.CompareNoCase( m_pEmbeddedFileRootFile->strObjectName ) == 0 )
		{
			// Object names are equal
			RegisteredObject* pRegisteredObject = FindRegisteredObjectByCLSID( m_pEmbeddedFileRootFile->guidDMClass );
			if( pRegisteredObject )
			{
				// Check to see if CLSID is type of object we want
				IDMUSProdDocType* pIDocTypeList;
				if( SUCCEEDED ( m_pIFramework8->FindDocTypeByNodeId( pRegisteredObject->guidNodeId, &pIDocTypeList ) ) )
				{
					if( pIDocTypeList == pIDocType )
					{
						pTheEmbeddedFile = m_pEmbeddedFileRootFile;
					}
				
					RELEASE( pIDocTypeList );
				}
			}

			if( pTheEmbeddedFile )
			{
				return pTheEmbeddedFile;
			}
		}
	}
	
	// Search m_lstEmbeddedFiles
	POSITION pos = m_lstEmbeddedFiles.GetHeadPosition();
	while( pos )
	{
		EmbeddedFile* pEmbeddedFile = m_lstEmbeddedFiles.GetNext( pos );

		if( strObjectName.CompareNoCase( pEmbeddedFile->strObjectName ) == 0 )
		{
			// Object names are equal
			RegisteredObject* pRegisteredObject = FindRegisteredObjectByCLSID( pEmbeddedFile->guidDMClass );
			if( pRegisteredObject )
			{
				// Check to see if CLSID is type of object we want
				IDMUSProdDocType* pIDocTypeList;
				if( SUCCEEDED ( m_pIFramework8->FindDocTypeByNodeId( pRegisteredObject->guidNodeId, &pIDocTypeList ) ) )
				{
					if( pIDocTypeList == pIDocType )
					{
						pTheEmbeddedFile =  pEmbeddedFile;
						RELEASE( pIDocTypeList );
						break;
					}
				
					RELEASE( pIDocTypeList );
				}
			}
		}
	}

	return pTheEmbeddedFile;
}
