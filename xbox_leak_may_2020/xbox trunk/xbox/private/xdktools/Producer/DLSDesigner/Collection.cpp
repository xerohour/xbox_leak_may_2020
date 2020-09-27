//
// Collection.cpp : implementation file
//

#include "stdafx.h"
#include "DlsDefsPlus.h"
#include "DLSDesignerDLL.h"
#include "DLSDesigner.h"

#include "Collection.h"
#include "Articulation.h"
#include "WaveNode.h"
#include "Wave.h"
#include "WaveDataManager.h"
#include "StereoWave.h"
#include "MonoWave.h"

#include "WaveCtl.h"
#include "InstrumentCtl.h"
#include "Instrument.h"
#include "region.h"
#include "resource.h"
#include "stream.h"

#include "DLSLoadSaveUtils.h"
#include "resource.h"

#include "CollectionPropPgMgr.h"
#include "CollectionPropPg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BOOL CCollection::m_bNoMIDIPort = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CCollection constructor/destructor

//////////////////////////////////////////////////////////////////////
//
//	CCollection::CCollection()
//
//////////////////////////////////////////////////////////////////////
CCollection::CCollection( CDLSComponent* pComponent )
{
	ASSERT( pComponent != NULL );

    m_lRef = 0;
	AddRef();

	m_pComponent = pComponent;
	m_pComponent->AddRef();

	m_pIDMCollection = NULL;

	m_pCollectionPropPgMgr = NULL;
	m_pIRootNode = NULL;
	m_pIParentNode = NULL;

    m_bIsDirty = FALSE;

	m_Instruments.m_pDLSComponent = pComponent;
	m_Waves.m_pDLSComponent = pComponent;

	m_Instruments.m_pCollection = this;
	m_Waves.m_pCollection = this;

	if(m_pCollectionPropPgMgr == NULL)
	{
		m_pCollectionPropPgMgr = new CCollectionPropPgMgr();
	}
	else
	{
		m_pCollectionPropPgMgr->AddRef();
	}

	// If no output port, warn the user
	if(m_pComponent->m_lstDLSPortDownload.IsEmpty())
	{
		if(m_bNoMIDIPort == FALSE)
		{
			AfxMessageBox(IDS_ERR_NO_PORT, MB_OK | MB_ICONEXCLAMATION);
		}
		
		m_bNoMIDIPort = TRUE;
	}
	else
	{
		m_bNoMIDIPort = FALSE;
	}


// J3 we need to make sure I initialize this to its proper default values 
// Currently I am using the values used by SynthAuthor
	m_rVersion.dwVersionMS = 1 << 16;
	m_rVersion.dwVersionLS = 1;
	m_fPatchResolution = 0;
	CoCreateGuid(&m_Guid);
}

CCollection::~CCollection()
{
	//Unload the waves and instruments before we die...
	Unload();

	// If we are in a Property Page Manager we need to remove ourselves before we go away
	IDMUSProdPropSheet *pIPropSheet;
	if(SUCCEEDED(m_pComponent->m_pIFramework->QueryInterface(IID_IDMUSProdPropSheet, (void**) &pIPropSheet)))
	{
		if(pIPropSheet->IsEqualPageManagerObject(this) == S_OK)
		{
			pIPropSheet->RemovePageManagerByObject(this);
		}
		else
		{
			if(m_pCollectionPropPgMgr)
			{
				m_pCollectionPropPgMgr->RemoveObject(this);
			}
		}
		pIPropSheet->Release();
	}

	if(m_pCollectionPropPgMgr)
	{
		DWORD refCount = m_pCollectionPropPgMgr->Release();
		if(refCount == 0)
		{
			m_pCollectionPropPgMgr = NULL;
		}
	}

	if( m_pComponent )
	{
		m_pComponent->Release();
	}

	ReleaseDMCollection();
}

/////////////////////////////////////////////////////////////////////////////
// CCollection IUnknown implementation

//////////////////////////////////////////////////////////////////////
//
//	CCollection::QueryInterface
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    if( ::IsEqualIID(riid, IID_IDMUSProdNode)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDMUSProdNode *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IPersist) )
    {
        AddRef();
        *ppvObj = (IPersist *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IPersistStream) )
    {
        AddRef();
        *ppvObj = (IPersistStream *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IDMUSProdPropPageObject) )
    {
        AddRef();
        *ppvObj = (IDMUSProdPropPageObject *)this;
        return S_OK;
    }

	if(::IsEqualIID(riid, IID_IDMUSProdNotifySink))
    {
        AddRef();
        *ppvObj = (IDMUSProdNotifySink*)this;
        return S_OK;
    }


    *ppvObj = NULL;
    return E_NOINTERFACE;
}

//////////////////////////////////////////////////////////////////////
//
//	CCollection::AddRef
//
//////////////////////////////////////////////////////////////////////
ULONG CCollection::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	AfxOleLockApp();
    return InterlockedIncrement( &m_lRef);
}

//////////////////////////////////////////////////////////////////////
//
//	CCollection::Release
//
//////////////////////////////////////////////////////////////////////
ULONG CCollection::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT( m_lRef != 0 );

	InterlockedDecrement( &m_lRef ); 

    if( m_lRef == 0 )
    {
        delete this;
        return 0;
    }
	AfxOleUnlockApp();
    return m_lRef;
}

/////////////////////////////////////////////////////////////////////////////
// CCollection IDMUSProdNode implementation

//////////////////////////////////////////////////////////////////////
//
//	CCollection IDMUSProdNode::GetNodeImageIndex	
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::GetNodeImageIndex(short* pnFirstImage)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pnFirstImage);

	ASSERT(m_pComponent != NULL);

	return(m_pComponent->GetCollectionImageIndex(pnFirstImage));
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::GetComponent
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( m_pComponent != NULL );

	return m_pComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::GetDocRootNode
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::GetDocRootNode( IDMUSProdNode** ppIRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pIRootNode )
	{
		m_pIRootNode->AddRef();
		*ppIRootNode = m_pIRootNode;
		return S_OK;
	}

	*ppIRootNode = NULL;
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::SetDocRootNode
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::SetDocRootNode( IDMUSProdNode* pIRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( pIRootNode != NULL );

	m_pIRootNode = pIRootNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::GetParentNode
//
//  Returns S_OK if there is a parent node - ppIParentnode then points
//  to the parent node. Else it will return failure and ppIParentNode
//  will be NULL
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(ppIParentNode);

	if(ppIParentNode == NULL)
	{
		return E_POINTER;
	}

// J3 This is the only one who can have a parent node of NULL
//	ASSERT( m_pIParentNode != NULL );

	// If there is a parent, set the pointer to point to it
    // and return ok else set it to null and return failure.
	if(m_pIParentNode)
	{		
    	*ppIParentNode = m_pIParentNode;
		m_pIParentNode->AddRef();
    	return S_OK;
	}
    else
    {
        *ppIParentNode = NULL;
        return E_FAIL;
    }
	
}


/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::SetParentNode
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	m_pIParentNode = pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::GetNodeId
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_CollectionNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::GetNodeName
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::GetNodeName(BSTR* pbstrName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pbstrName);

    *pbstrName = m_Info.m_csName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::GetNodeNameMaxLength
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::GetNodeNameMaxLength(short* pnMaxLength)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pnMaxLength);

	*pnMaxLength = BUFFER_256;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::ValidateNodeName
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::ValidateNodeName(BSTR bstrName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;

	strName = bstrName;
	
	::SysFreeString(bstrName);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::SetNodeName
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::SetNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CString strName = bstrName;
	::SysFreeString( bstrName );

	m_Info.m_csName = strName;

	// Set flag so we know to save file 
	SetDirtyFlag();

	if(m_pCollectionPropPgMgr)
	{
		IDMUSProdPropSheet* pIPropSheet;
		if( SUCCEEDED ( m_pComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			if( pIPropSheet->IsShowing() == S_OK && pIPropSheet->IsEqualPageManagerObject(this) == S_OK)
			{
				m_pCollectionPropPgMgr->RefreshData();
				m_pCollectionPropPgMgr->m_pCollectionPage->OnSetActive();
			}

			RELEASE(pIPropSheet);
		}
	}

	// Notify All the waves and instruments that the collection name has changed
	// so they can change it in the transport drop-down
	CWaveNode* pWaveNode = m_Waves.GetHead();
	for ( ; pWaveNode != NULL; pWaveNode = pWaveNode->GetNext())
	{
		pWaveNode->SetTransportName();
	}

	CInstrument* pInstrument = m_Instruments.GetHead();
	for( ; pInstrument != NULL; pInstrument = pInstrument->GetNext())
	{
		if(pInstrument->m_pInstrumentCtrl)
			pInstrument->m_pInstrumentCtrl->SetTransportName();
	}

	if(m_pComponent->m_pIFramework)
		m_pComponent->m_pIFramework->NotifyNodes(this, COLLECTION_NameChange, NULL);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::GetEditorClsId
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::GetEditorClsId(CLSID* pClsId)
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::GetEditorTitle
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::GetEditorTitle(BSTR* pbstrTitle)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::GetEditorWindow
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::GetEditorWindow(HWND* hWndEditor)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::SetEditorWindow
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::SetEditorWindow(HWND hWndEditor)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::UseOpenCloseImages
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::UseOpenCloseImages(BOOL* pfUseOpenCloseImages)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pfUseOpenCloseImages);
	
	*pfUseOpenCloseImages = TRUE;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::GetRightClickMenuId
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::GetRightClickMenuId(HINSTANCE* phInstance, UINT* pnMenuId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pnMenuId);
	ASSERT(phInstance);

	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_COLLECTION_NODE_RMENU;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::OnRightClickMenuInit
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::OnRightClickMenuInit(HMENU hMenu)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

#ifdef DMP_XBOX
	::EnableMenuItem( hMenu, IDM_IMPORT_CONFIGS, MF_GRAYED | MF_BYCOMMAND );
	::EnableMenuItem( hMenu, IDM_EXPORT_CONFIGS, MF_GRAYED | MF_BYCOMMAND );
#endif

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::OnRightClickMenuSelect
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pComponent != NULL);
	ASSERT(m_pComponent->m_pIFramework != NULL);

	HRESULT hr = E_FAIL;
	
	switch( lCommandId )
	{
		case IDM_RENAME:
		{
			if(SUCCEEDED(m_pComponent->m_pIFramework->EditNodeLabel((IDMUSProdNode *)this)))
			{
				hr = S_OK;
			}
			break;
		}
		
		case IDM_PROPERTIES:
		{
			if(SUCCEEDED(OnShowProperties()))
			{
				hr = S_OK;
			}
			break;
		}

		case IDM_VERIFY_DLS1:
		{
			if(!VerifyDLS())
			{
				AfxMessageBox(IDS_NOT_DLS1);
			}
			else
			{
				AfxMessageBox(IDS_IS_DLS1);
			}

			hr = S_OK;
			break;
		}
		
		case IDM_EXPORT_CONFIGS:
		{
			BSTR bstrFilePath = NULL;
			if(SUCCEEDED(m_pComponent->m_pIFramework->GetNodeFileName(this, &bstrFilePath)))
			{
				CString sFilePath = bstrFilePath;
				SysFreeString(bstrFilePath);
				
				// Take out the filename we need just the path
				int nLength = sFilePath.GetLength();
				int nLastSlash = sFilePath.ReverseFind('\\');
				sFilePath = sFilePath.Left(nLastSlash);
	
				// Export to [Path]\Producer.POL - policy file
				m_pComponent->ExportConfigurations(sFilePath);
			}

			break;
		}

		case IDM_IMPORT_CONFIGS:
		{
			BSTR bstrFilePath = NULL;
			if(SUCCEEDED(m_pComponent->m_pIFramework->GetNodeFileName(this, &bstrFilePath)))
			{
				CString sFilePath = bstrFilePath;
				SysFreeString(bstrFilePath);
				
				// Take out the filename we need just the path
				int nLength = sFilePath.GetLength();
				int nLastSlash = sFilePath.ReverseFind('\\');
				sFilePath = sFilePath.Left(nLastSlash);
	
				// Import [Path]\Producer.POL - policy file
				m_pComponent->ImportConfigurations(sFilePath);
			}

			break;
		}

	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::DeleteChildNode
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_FAIL;	// Cannot delete folders under Collection
}


/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::InsertChildNode
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::InsertChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_FAIL;	// Cannot insert additional folders under Collection
}

////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::DeleteNode
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::DeleteNode( BOOL fPromptUser )
{
	ASSERT(m_pComponent != NULL);
	if (NULL == m_pComponent)
	{
		return E_UNEXPECTED;
	}

	IDMUSProdFramework* pIFramework = m_pComponent->m_pIFramework;
	ASSERT(NULL != pIFramework);
	if (NULL == pIFramework)
	{
		return E_UNEXPECTED;
	}

	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	IDMUSProdNode* pIParentNode;

	if( SUCCEEDED ( GetParentNode(&pIParentNode) ) )
	{
		if( pIParentNode )
		{
			HRESULT hr = pIParentNode->DeleteChildNode( this, fPromptUser );
			pIParentNode->Release();
			return hr;		
		}
	}

	// We need to close out all the open file handles
	CWaveNode* pWaveNode = m_Waves.GetHead();
	while(pWaveNode)
	{
		if(pWaveNode->GetFileName() == m_sFileName)
		{
			CWave* pWave = pWaveNode->GetWave();
			ASSERT(pWave);
			if(FAILED(pWave->CloseSourceHandler()))
			{
				return E_FAIL;
			}
		}
		pWaveNode = pWaveNode->GetNext();
	}

	// Remove from Project Tree
	if(m_pComponent->m_pIFramework->RemoveNode(this, fPromptUser) == S_FALSE)
	{
		// The user canselled the delete, so we need to attach waves
		// back to their original source file handlers.
		CWaveNode* pWaveNode = m_Waves.GetHead();
		while(pWaveNode)
		{
			if(pWaveNode->GetFileName() == m_sFileName)
			{
				CWave* pWave = pWaveNode->GetWave();
				ASSERT(pWave);
				if(FAILED(pWave->OnSourceRenamed(m_sFileName)))
				{
					return E_FAIL;
				}
			}
			pWaveNode = pWaveNode->GetNext();
		}
		return E_FAIL;	
	}

	// Unregister all the waves from the transport
	m_Waves.UnRegisterWavesFromTransport();

	// Ask the wave nodes to release their dummy instruments
	m_Waves.ReleaseWaveNodeDummyInstruments();
	
	// Remove from Component DLS list...and call a release as well...
	m_pComponent->DeleteFromCollectionList(this);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CCollection IDMUSProdNode::GetFirstChild
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::GetFirstChild(IDMUSProdNode** ppIFirstChildNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(ppIFirstChildNode == NULL)
	{
		return E_POINTER;
	}

	*ppIFirstChildNode = NULL;

	IDMUSProdNode* pINode = static_cast<IDMUSProdNode*>(&m_Instruments);

	if(pINode)
	{
		pINode->AddRef();
		*ppIFirstChildNode = pINode;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CCollection IDMUSProdNode::GetNextChild
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::GetNextChild(IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(ppINextChildNode == NULL)
	{
		return E_POINTER;
	}

	*ppINextChildNode = NULL;

	if(pIChildNode == NULL)
	{
		return E_INVALIDARG;
	}

	IDMUSProdNode* pINode = static_cast<IDMUSProdNode*>(&m_Waves);

	if(pINode && pINode != pIChildNode)
	{
		pINode->AddRef();
		*ppINextChildNode = pINode;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CCollection IDMUSProdNode::GetNodeListInfo
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::GetNodeListInfo(DMUSProdListInfo* pListInfo)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pListInfo == NULL )
	{
		return E_POINTER;
	}

	CString strDescriptor;

    pListInfo->bstrName = m_Info.m_csName.AllocSysString();
    pListInfo->bstrDescriptor = strDescriptor.AllocSysString();
	memcpy( &pListInfo->guidObject, &m_Guid, sizeof(GUID) );

	// Must check pListInfo->wSize before populating additional fields
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::OnNodeSelChanged
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::OnNodeSelChanged( BOOL fSelected )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::CreateDataObject
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( m_pComponent != NULL );

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppIDataObject = NULL;

	// Create the CDllJazzDataObject 
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	IStream* pIStream;

    // Save collection into stream
	HRESULT hr = E_FAIL;
	if( SUCCEEDED ( m_pComponent->m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		if( SUCCEEDED ( Save( pIStream, FALSE ) ) )
        {
            // Place the format in the CDllJazzdataobject
			if( SUCCEEDED ( pDataObject->AddClipFormat( m_pComponent->m_cfCollection, pIStream ) ) )
			{
				hr = S_OK;
			}
		}
		pIStream->Release();
	}

	if( SUCCEEDED ( hr ) )
	{
		hr = E_FAIL;

		// Create a stream in CF_DMUSPROD_FILE format
		if( SUCCEEDED ( m_pComponent->m_pIFramework->SaveClipFormat( m_pComponent->m_cfProducerFile, this, &pIStream ) ) )
		{
			// Collection nodes represent files so we must also
			// place CF_DMUSPROD_FILE into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( m_pComponent->m_cfProducerFile, pIStream ) ) )
			{
				hr = S_OK;
			}

			pIStream->Release();
		}
	}

	if( SUCCEEDED ( hr ) )
	{
		*ppIDataObject = pDataObject;	// already AddRef'd
	}
	else
	{
		pDataObject->Release();
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::CanCut
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::CanCut()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return CanDelete();

}

/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::CanCopy
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::CanCopy()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    //return E_NOTIMPL;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::CanDelete
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::CanDelete()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::CanDeleteChildNode
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::CanPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pfWillSetReference == NULL )
	{
		return E_POINTER;
	}

	*pfWillSetReference = FALSE;

    if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}
	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
    
    HRESULT hr = E_FAIL;

	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_pComponent->m_cfInstrument)))
        hr = S_OK;

	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_pComponent->m_cfWave )))
        hr = S_OK;
	
    pDataObject->Release();

    return hr;

}

/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::PasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject to get the data object's stream.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = E_FAIL;

	if(SUCCEEDED(pDataObject->IsClipFormatAvailable( pIDataObject, m_pComponent->m_cfInstrument)))
    {
		hr = m_Instruments.PasteCF_INSTRUMENT(pDataObject, pIDataObject);
		pDataObject->Release();
		return hr;
    }

    if(SUCCEEDED(pDataObject->IsClipFormatAvailable( pIDataObject, m_pComponent->m_cfWave)))
    {
		hr = m_Waves.PasteCF_WAVE(pDataObject, pIDataObject);
		pDataObject->Release();
		return hr;
    }

	pDataObject->Release();
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::CanChildPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
										    BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;   

}

/////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNode::ChildPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;

}

/////////////////////////////////////////////////////////////////////////////
// CCollection IDMUSProdNode::GetObject

HRESULT CCollection::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		return E_UNEXPECTED;
	}
	
	// Must be asking for an interface to a CLSID_DirectMusic object
	if( ::IsEqualCLSID( rclsid, CLSID_DirectMusic ) )
	{
		if( m_pComponent->m_pIDirectMusic )
		{
			return m_pComponent->m_pIDirectMusic->QueryInterface( riid, ppvObject );
		}
	}
	
	if(::IsEqualCLSID(rclsid, CLSID_DirectMusicCollection))
	{
		 if(m_pIDMCollection == NULL)
		 {
			 if(FAILED(CreateDMCollection()))
			 {
				 *ppvObject = NULL;
				 return E_FAIL;
			 }
		 }

		 // AddRef it before passing it on....
		 m_pIDMCollection->AddRef();
		 *ppvObject = m_pIDMCollection;
		 return S_OK;
	}

	*ppvObject = NULL;
	return E_NOINTERFACE;
}

//////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNotifySink Implementation
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//
// CCollection IDMUSProdNotifySink::OnUpdate
//
//////////////////////////////////////////////////////////////////////////////
HRESULT CCollection::OnUpdate(IDMUSProdNode* pIDocRootNode, GUID guidUpdateType, void* pvData)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(pIDocRootNode);
	if(pIDocRootNode == NULL)
	{
		return E_POINTER;
	}

	if(pIDocRootNode != m_pIRootNode)
	{
		return E_NOTIMPL;
	}


	if(::IsEqualGUID( guidUpdateType, FRAMEWORK_BeforeFileSave))
	{
		ASSERT(pvData);
		if(pvData == NULL)
		{
			return E_POINTER;
		}

		WCHAR* pwszFileName = (WCHAR*)pvData;
		CString sFileName = pwszFileName;

		if(m_sFileName != sFileName)
		{
			return S_OK;
		}

		// Go through all wavenodes in the collection and close out the source handlers
		// Then rename the collection and reset the source handlers to point to and read 
		// from the renamed temporary collection file...
		CWaveNode* pWaveNode = m_Waves.GetHead();
		while(pWaveNode)
		{
			if(pWaveNode->IsSourceInTempFile() == FALSE && pWaveNode->GetFileName() == m_sFileName)
			{
				CWave* pWave = pWaveNode->GetWave();
				ASSERT(pWave);
				if(FAILED(pWave->CloseSourceHandler()))
				{
					return E_FAIL;
				}
			}

			pWaveNode = pWaveNode->GetNext();
		}

		// Get a temp file name
		char szTempPath[MAX_PATH];
		DWORD dwSuccess = GetTempPath(MAX_PATH, szTempPath);
		CString sTempPath = szTempPath;

		CString sPrefix = "DMP";
		CString sExt = "tmp";
		char szTempName[MAX_PATH];

		if(FAILED(CWaveDataManager::CreateUniqueTempFile(sPrefix, sExt, m_sTempName)))
		{
			GetTempFileName(sTempPath, sPrefix, 0, szTempName);
			m_sTempName = szTempName;
		}

		// We will create a new file when we rename
		DeleteFile(m_sTempName);

		try
		{
			CFile::Rename(sFileName, m_sTempName);
		}
		catch(CFileException e)
		{
			return E_FAIL;
		}

		pWaveNode = m_Waves.GetHead();
		while(pWaveNode)
		{
			if(pWaveNode->IsSourceInTempFile() == FALSE && pWaveNode->GetFileName() == m_sFileName)
			{
				CWave* pWave = pWaveNode->GetWave();
				ASSERT(pWave);

				if(FAILED(pWave->OnSourceRenamed(m_sTempName)))
				{
					return E_FAIL;
				}
			}
			pWaveNode = pWaveNode->GetNext();
		}
		
		return S_OK;
	}
	
	if(::IsEqualGUID( guidUpdateType, FRAMEWORK_AfterFileSave))
	{
		ASSERT(pvData);
		if(pvData == NULL)
		{
			return E_POINTER;
		}

		WCHAR* pwszFileName = (WCHAR*)pvData;
		CString sFileName = pwszFileName;

		if(m_sFileName != sFileName)
		{
			return S_OK;
		}

		CWaveNode* pWaveNode = m_Waves.GetHead();
		while(pWaveNode)
		{
			CWave* pWave = pWaveNode->GetWave();
			ASSERT(pWave);
			if(FAILED(pWave->CleanupAfterSave(sFileName)))
			{
				return E_FAIL;
			}
			pWaveNode = pWaveNode->GetNext();
		}

		// Delete the temp file created before the save
		DeleteFile(m_sTempName);

		return S_OK;
	}

	if(::IsEqualGUID( guidUpdateType, FRAMEWORK_BeforeFileNameChange))
	{
		// Go through all wavenodes in the collection and close out the source handlers
		// Then rename the collection and reset the source handlers to point to and read 
		// from the renamed temporary collection file...
		CWaveNode* pWaveNode = m_Waves.GetHead();
		while(pWaveNode)
		{
			if(pWaveNode->IsSourceInTempFile() == FALSE && pWaveNode->GetFileName() == m_sFileName)
			{
				CWave* pWave = pWaveNode->GetWave();
				ASSERT(pWave);
				if(FAILED(pWave->CloseSourceHandler()))
				{
					return E_FAIL;
				}
			}
 
			pWaveNode = pWaveNode->GetNext();
		}


		return S_OK;
	}

	if(::IsEqualGUID( guidUpdateType, FRAMEWORK_AbortFileNameChange))
	{
		CWaveNode* pWaveNode = m_Waves.GetHead();
		while(pWaveNode)
		{
			if(pWaveNode->GetFileName() == m_sFileName)
			{
				CWave* pWave = pWaveNode->GetWave();
				ASSERT(pWave);

				if(FAILED(pWave->OnSourceRenamed(m_sTempName)))
				{
					return E_FAIL;
				}
			}
			pWaveNode = pWaveNode->GetNext();
		}

		return S_OK;
	}

	if(::IsEqualGUID(guidUpdateType, FRAMEWORK_FileNameChange))
	{
		ASSERT(m_pComponent->m_pIFramework);
		if(m_pComponent->m_pIFramework == NULL)
		{
			return E_FAIL;
		}

		// Ge the new file name for the node
		BSTR bstrNodeFileName;
		if(FAILED(m_pComponent->m_pIFramework->GetNodeFileName((IDMUSProdNode*)this, &bstrNodeFileName)))
		{
			return E_FAIL;
		}

		CString sFileName = bstrNodeFileName;
		::SysFreeString(bstrNodeFileName);

		// Are we already looking at the same file?
		if(m_sFileName == sFileName)
		{
			return S_OK;
		}
		
		CWaveNode* pWaveNode = m_Waves.GetHead();
		while(pWaveNode)
		{
			// Keep the name with the wave node...
			pWaveNode->SetFileName(sFileName);
			CWave* pWave = pWaveNode->GetWave();
			ASSERT(pWave);

			EnterCriticalSection(&pWaveNode->m_CriticalSection);
			if(FAILED(pWave->FileNameChanged(sFileName)))
			{
				LeaveCriticalSection(&pWaveNode->m_CriticalSection);
				return E_FAIL;
			}
			
			LeaveCriticalSection(&pWaveNode->m_CriticalSection);

			pWaveNode = pWaveNode->GetNext();
		}

		m_sFileName = sFileName;

		return S_OK;
	}


	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CCollection IPersist implementation

/////////////////////////////////////////////////////////////////////////////
//
// CCollection IPersist::GetClassID
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pClsId != NULL );

    memset( pClsId, 0, sizeof( CLSID ) );

    return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CCollection IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
//
// CCollection IPersistStream::IsDirty
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::IsDirty()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return m_bIsDirty ? S_OK : S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
//
// CCollection IPersistStream::Load
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::Load( IStream* pIStream )
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;
    HRESULT hr;

    ASSERT( pIStream != NULL );

    hr = AllocRIFFStream( pIStream, &pIRiffStream );

    if( SUCCEEDED( hr ) )
    {
        ckMain.fccType = FOURCC_DLS;

        if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
        {
            hr = Load( pIRiffStream, &ckMain );
			m_bIsDirty = FALSE;
			if(SUCCEEDED(hr))
			{
				m_pComponent->DeleteFromFailedCollectionGUIDs(m_Guid);
				//VerifyDLS();
			}
        }
		else
		{
			hr = E_FAIL;
		}

        pIRiffStream->Release();
    }

	if(FAILED(hr))
	{
		m_pComponent->AddToFailedCollectionGUIDs(m_Guid);
	}


    return hr;
}


/////////////////////////////////////////////////////////////////////////////
//
// CCollection IPersistStream::Save
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::Save(IStream* pIStream, BOOL fClearDirty)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    IDMUSProdRIFFStream* pIRiffStream;
    HRESULT hr;

    hr = E_FAIL;

    if(SUCCEEDED(AllocRIFFStream(pIStream, &pIRiffStream)))
    {
		if(SUCCEEDED(Save(pIRiffStream, true)))
        {
			if(fClearDirty)
			{
				ClearDirtyFlag();
			}
			hr = S_OK;
		}
		pIRiffStream->Release();
	}
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollection IPersistStream::GetSizeMax
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::GetSizeMax( ULARGE_INTEGER FAR* /*pcbSize*/ )
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollection::Save
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollection::Save( const char* strFileName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);		
	HRESULT	hr = S_OK;
    IStream*	pStream = NULL;
	IDMUSProdRIFFStream*	pRiffStream = NULL;

	hr = AllocStreamToFile( &pStream, strFileName, 0 );
	if( SUCCEEDED(hr) )
	{
	    hr = AllocRIFFStream( pStream, &pRiffStream );
	    if (SUCCEEDED(hr))
	    {
            hr = Save(pRiffStream, TRUE);
			pRiffStream->Release();
        }
		pStream->Release();
    }
	return hr;
}

//////////////////////////////////////////////////////////////////////
//
// CCollection::ResolveConnections()
//
//////////////////////////////////////////////////////////////////////
void CCollection::ResolveConnections()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);			

    SetWaveIDs();
    
	CPtrList waveList;			// list of mono waves
	if(FAILED(m_Waves.GetListOfMonoWaves(&waveList)))
		return;

	CInstrument* pInstrument = m_Instruments.GetHead();
    for( ; pInstrument != NULL; pInstrument = pInstrument->GetNext())
    {
        CRegion* pRegion = pInstrument->m_Regions.GetHead();
        for ( ; pRegion != NULL; pRegion = pRegion->GetNext())
        	{
        	CWave* pWave = NULL; // wave we'll assign to the region

			// skip this instrument if its waves are already assigned
        	if (pRegion->m_pWave != NULL)
				{
			#ifdef _DEBUG
				// verify that all other regions in the instrument have waves assigned
				while ((pRegion = pRegion->GetNext()) != NULL)
					ASSERT(pRegion->m_pWave != NULL);
			#endif
				break;
				}

			POSITION position = waveList.GetHeadPosition();
            while (position)
            	{
				CMonoWave* pMonoWave = (CMonoWave*)waveList.GetNext(position);
				ASSERT(pMonoWave);
                if (pRegion->m_rWaveLink.ulTableIndex == pMonoWave->GetWaveID())
                	{
                    pWave = pMonoWave;
                    break;
	                }
    	        }
			
			// If no wave was found for this region we assign the first wave
			if (pWave == NULL  && !waveList.IsEmpty()) 
				{
				pWave = (CWave*)waveList.GetHead();
				ASSERT(pWave);
				}

			// if there are regions further in the list with the same ID, merge them with this one
			pRegion->MergeNextRegions(pWave);

			// finally, assign wave to region
			if (pWave)
				{
                pRegion->m_pWave = pWave;
                pRegion->m_rWaveLink.ulTableIndex = pWave->GetWaveID();
				pRegion->GetName();
                pWave->GetNode()->AddRef();
				}
    	    }
		}
}

/////////////////////////////////////////////////////////////////////////////
// CCollection Additional functions


/////////////////////////////////////////////////////////////////////////////
// CCollection::SetWaveIDs

void CCollection::SetWaveIDs()
{
    DWORD dwID = 0;
	CPtrList waveList;

	if(FAILED(m_Waves.GetListOfMonoWaves(&waveList)))
		return;

	if(waveList.IsEmpty())
		return;

	// assign IDs to mono waves, starting at 0, as required by DLS spec
	POSITION position = waveList.GetHeadPosition();
	while(position)
		{
		CMonoWave* pMonoWave = (CMonoWave*)waveList.GetNext(position);
		ASSERT(pMonoWave);

		pMonoWave->SetWaveID(dwID++);
		}

	waveList.RemoveAll();
	if(FAILED(m_Waves.GetListOfWaves(&waveList)))
		return;

	// assign IDs to all other waves, to facilitate internal save & load
	position = waveList.GetHeadPosition();
	while(position)
		{
		CWave* pWave = (CWave*)waveList.GetNext(position);
		ASSERT(pWave);

		if (pWave->GetChannelCount() > 1)
			pWave->SetWaveID(dwID++);
		}
}

/////////////////////////////////////////////////////////////////////////////
// CCollection::PrepareConnections

void CCollection::PrepareConnections()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);		
 
	SetWaveIDs();

	CInstrument *pInstrument = m_Instruments.GetHead();
    for ( ; pInstrument != NULL; pInstrument = pInstrument->GetNext())
    {
        CRegion *pRegion = pInstrument->m_Regions.GetHead();
        for ( ; pRegion != NULL; pRegion = pRegion->GetNext())
        {
            if (pRegion->m_pWave != NULL)
            {
				pRegion->m_rWaveLink.ulTableIndex = pRegion->m_pWave->GetWaveID();
            }
        }
    }
}


/////////////////////////////////////////////////////////////////////////////
// CCollection::Save

HRESULT CCollection::Save(IDMUSProdRIFFStream* pIRiffStream, BOOL fFullSave)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pIRiffStream);

    IStream* pIStream = pIRiffStream->GetStream();

	ASSERT(pIStream);

	HRESULT			hr = S_OK;			// Return code
	MMCKINFO		ckMain;
	MMCKINFO		ck;
    MMCKINFO        dck;
	DWORD			cb;
	CInstrument *	pInstrument;
	CWaveNode*		pWaveNode = NULL;
    CWave*			pWave = NULL;

    SetWaveIDs();

	ckMain.fccType = mmioFOURCC('D','L','S',' ');
	hr = pIRiffStream->CreateChunk(&ckMain, MMIO_CREATERIFF);
    if (FAILED(hr))
	{
		pIStream->Release();
        return (hr);
    }

    ck.ckid = FOURCC_DLID ;
    hr = pIRiffStream->CreateChunk(&ck, 0);
    if (FAILED(hr))
    {
		pIStream->Release();
        return (hr);
    }

	if (m_Guid != GUID_NULL)
	{
		hr = pIStream->Write((LPSTR) &m_Guid, sizeof(GUID), &cb);	
		if (FAILED(hr) || cb != sizeof(GUID))
		{
			pIStream->Release();
			return (hr);
		}
	}

	if (FAILED(pIRiffStream->Ascend(&ck, 0)))
    {
		pIStream->Release();
        return (E_FAIL);
    }

    ck.ckid = FOURCC_COLH ;
    hr = pIRiffStream->CreateChunk(&ck, 0);
    if (FAILED(hr))
    {
		pIStream->Release();
        return (hr);
    }

    m_rDLSHeader.cInstruments = m_Instruments.GetCount();

    hr = pIStream->Write((LPSTR) &m_rDLSHeader, sizeof(DLSHEADER), &cb);	
    if (FAILED(hr) || cb != sizeof(DLSHEADER))
    {
		pIStream->Release();
        return (hr);
    }
    
	if (FAILED(pIRiffStream->Ascend(&ck, 0)))
    {
		pIStream->Release();
        return (E_FAIL);
    }
    
	ck.ckid = FOURCC_VERS ;
    hr = pIRiffStream->CreateChunk(&ck, 0);
    if (FAILED(hr))
    {
		pIStream->Release();
        return (hr);
    }
    hr = pIStream->Write((LPSTR) &m_rVersion, sizeof(DLSVERSION), &cb);	
    if (FAILED(hr) || cb != sizeof(DLSVERSION))
    {
		pIStream->Release();
        return (hr);
    }
    if (FAILED(pIRiffStream->Ascend(&ck, 0)))
    {
		pIStream->Release();
        return (E_FAIL);
    }

	if (FAILED(pIRiffStream->Ascend(&ck, 0)))
	{
		pIStream->Release();
		return (E_FAIL);
	}

	dck.fccType = FOURCC_LINS ;
	hr = pIRiffStream->CreateChunk(&dck,MMIO_CREATELIST);
	if (FAILED(hr))
	{
		pIStream->Release();
		return (hr);
	}

	PrepareConnections();
	pInstrument = m_Instruments.GetHead();
	for ( ; pInstrument; pInstrument = pInstrument->GetNext())
	{
		hr = pInstrument->Save(pIRiffStream, TRUE);
		if (FAILED(hr)) 
		{
			pIStream->Release();
			return(hr);
		}
	}

	pIRiffStream->Ascend(&dck, 0);
	if (fFullSave)
	{
		ck.ckid = FOURCC_PTBL ;
		hr = pIRiffStream->CreateChunk(&ck, 0);
		if (FAILED(hr))
		{
			pIStream->Release();
			return (hr);
		}

		POOLTABLE table;
		table.cbSize = sizeof(POOLTABLE);
		
		// Get the list of all mono waves
		CPtrList waveList;
		if(FAILED(m_Waves.GetListOfMonoWaves(&waveList)))
		{
			pIStream->Release();
			return E_OUTOFMEMORY;
		}

		table.cCues = waveList.GetCount();
		hr = pIStream->Write((LPSTR) &table,sizeof(POOLTABLE), &cb);	
		if (FAILED(hr) || cb != sizeof(POOLTABLE))
		{
			pIStream->Release();
			return (hr);
		}

		unsigned long ulTablePos;
		unsigned long ulPoolEnd;
		unsigned long ulWaveStart;

		ulTablePos = GetFilePos(pIStream);

		POSITION position = waveList.GetHeadPosition();
		while(position)
		{
			POOLCUE cue;
			cue.ulOffset = 0;
			hr = pIStream->Write((LPSTR) &cue,sizeof(POOLCUE), &cb);	
			if (FAILED(hr) || cb != sizeof(POOLCUE))
			{
				pIStream->Release();
				return (hr);
			}
			waveList.GetNext(position);
		}

		if (FAILED(pIRiffStream->Ascend(&ck, 0)))
		{
			pIStream->Release();
			return (E_FAIL);
		}

		dck.fccType = FOURCC_WVPL ;
		hr = pIRiffStream->CreateChunk(&dck,MMIO_CREATELIST);
		if (FAILED(hr))
		{
			pIStream->Release();
			return (hr);
		}
		ulWaveStart = GetFilePos(pIStream);
		pWaveNode = m_Waves.GetHead();
		for ( ; pWaveNode; pWaveNode = pWaveNode->GetNext())
		{
			CWave* pWave = pWaveNode->GetWave();
			ASSERT(pWave);

			hr = pWave->Save(pIRiffStream, MMIO_CREATELIST, TRUE);
			if (FAILED(hr)) 
			{
				pIStream->Release();
				return(hr);
			}
		}

		pIRiffStream->Ascend(&dck, 0);
		ulPoolEnd = GetFilePos(pIStream);
		SetFilePos(pIStream, ulTablePos);

		position = waveList.GetHeadPosition();
		while(position)
		{
			CMonoWave* pWave = (CMonoWave*)waveList.GetNext(position);
			ASSERT(pWave);
			
			POOLCUE cue;
			
			if(pWave)
			{
				cue.ulOffset = pWave->GetWavePoolOffset() - ulWaveStart;
				hr = pIStream->Write((LPSTR) &cue, sizeof(POOLCUE), &cb);	
				if (FAILED(hr) || cb != sizeof(POOLCUE))
				{
					pIStream->Release();
					return (hr);
				}
			}
		}

		SetFilePos(pIStream, ulPoolEnd);
		m_Info.Save(pIRiffStream);
        m_UnknownChunk.Save(pIRiffStream);
	}
    pIRiffStream->Ascend(&ckMain, 0);
	pIStream->Release();
    return (hr);
}

/////////////////////////////////////////////////////////////////////////////
// CCollection::Download
HRESULT CCollection::Download()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	HRESULT	hr = S_OK;
	
	if(m_pComponent->m_lstDLSPortDownload.IsEmpty())
	{
		return E_FAIL;
	}

	if(m_pComponent->IsAutoDownloadOkay() == FALSE)
	{
		// Okay not to download anything if preference set that way
		return S_OK;
	}
	

	POSITION position = m_pComponent->m_lstDLSPortDownload.GetHeadPosition();
	while(position)
	{
		IDirectMusicPortDownload* pIDMPortDownload = m_pComponent->m_lstDLSPortDownload.GetNext(position);

		CWaveNode* pWaveNode = m_Waves.GetHead();
		for ( ; pWaveNode != NULL; pWaveNode = pWaveNode->GetNext())
		{
			hr = pWaveNode->DM_Init(pIDMPortDownload);
			if (SUCCEEDED(hr))
			{
				hr = pWaveNode->Download(pIDMPortDownload);
				if ( !SUCCEEDED(hr) )
				{
					if(hr & (DMUS_E_INSUFFICIENTBUFFER | DMUS_E_BUFFERNOTSET | DMUS_E_BUFFERNOTAVAILABLE))
					{
						/*CString sPortName;
						m_pComponent->GetPortName(pIDMPortDownload, sPortName);
						CString sErrorMessage;
						sErrorMessage.Format(IDS_ERR_FAILEDBUFFER,sPortName);
						AfxMessageBox(sErrorMessage);*/

						// ashtat - BUGBUG!! Needs a better message - see Manbugs:33331
						AfxMessageBox(IDS_ERR_DOWNLOAD_DLS_CONFIG);
						break;
					}
					AfxMessageBox(IDS_ERR_INIT_DOWNLOAD,MB_OK | MB_ICONEXCLAMATION);
				}
			}
			else if(hr & E_OUTOFMEMORY)
			{
				AfxMessageBox(IDS_ERR_WAVE_DOWNLOAD_FAILED, MB_OK | MB_ICONEXCLAMATION);

				// Does it make sense to keep trying to download the rest of the 
				// waves instead of breaking out of this loop? What if there isn't
				// sufficient memory to download a huge wave but other smaller 
				// waves can still be downloaded, right? 
				// Also should we continue to try and download the instruments once 
				// this has failed?? The user will not be able to hear anything in the 
				// instruments that refer to waves that haven't been downloaded
				break;
			}
		}

		CInstrument* pInstrument = m_Instruments.GetHead();
		for( ; pInstrument != NULL; pInstrument = pInstrument->GetNext())
		{
			hr = pInstrument->DM_Init(pIDMPortDownload);
			if (SUCCEEDED(hr))
			{
				hr = pInstrument->Download(pIDMPortDownload);
				if ( !SUCCEEDED(hr) )
				{
					if(hr & (DMUS_E_INSUFFICIENTBUFFER | DMUS_E_BUFFERNOTSET | DMUS_E_BUFFERNOTAVAILABLE))
					{
						/*CString sPortName;
						m_pComponent->GetPortName(pIDMPortDownload, sPortName);
						CString sErrorMessage;
						sErrorMessage.Format(IDS_ERR_FAILEDBUFFER,sPortName);
						AfxMessageBox(sErrorMessage);*/
						// ashtat - BUGBUG!! Needs a better message - see Manbugs:33331
						
						AfxMessageBox(IDS_ERR_DOWNLOAD_DLS_CONFIG);
						break;
					}
					AfxMessageBox(IDS_ERR_INIT_DOWNLOAD,MB_OK | MB_ICONEXCLAMATION);
				}
				else if(hr & E_OUTOFMEMORY)
				{
					AfxMessageBox(IDS_ERR_INST_DOWNLOAD_FAILED, MB_OK | MB_ICONEXCLAMATION);

					// It does make sense to NOT download anymore instruments 
					// once we run out memory since most instruments will 
					// require approximately same memory (though the size will 
					// vary depending on the number of regions and articulations 
					// but not as much as it will for waves right?).
					break;

				}

				pInstrument->UpdatePatch();
			}
		}
	}

	return hr;
}


//////////////////////////////////////////////////////////////////////////////
// CCollection::Unload - "Undownloads" all the wavs and instruments from the port
void CCollection::Unload()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_pComponent->m_lstDLSPortDownload.IsEmpty())
		return;

	POSITION position = m_pComponent->m_lstDLSPortDownload.GetHeadPosition();
	while(position)
	{
		IDirectMusicPortDownload* pIDMPortDownload = m_pComponent->m_lstDLSPortDownload.GetNext(position);		
		ASSERT(pIDMPortDownload);

		// ALWAYS unload instruments BEFORE waves 
		CInstrument* pInstrument = m_Instruments.GetHead();
		for( ; pInstrument != NULL; pInstrument = pInstrument->GetNext())
		{
			pInstrument->Stop(true);
			pInstrument->Unload(pIDMPortDownload);
		}
		
		CWaveNode* pWaveNode = m_Waves.GetHead();
		for ( ; pWaveNode != NULL; pWaveNode = pWaveNode->GetNext())
		{
			// Stop the wave if it's playing 
			pWaveNode->Stop(true);
			pWaveNode->Unload(pIDMPortDownload);
		}
	}
}



//////////////////////////////////////////////////////////////////////////////
// CCollection::Load

HRESULT CCollection::Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pComponent->m_pIFramework != NULL);

	ASSERT( theApp.m_hKeyProgressBar == NULL );	// Should not happen!
	if( theApp.m_hKeyProgressBar )				// Just in case....
	{
		m_pComponent->m_pIFramework->EndProgressBar( theApp.m_hKeyProgressBar );
		theApp.m_hKeyProgressBar = NULL;
	}

	// Start the progress bar
	CString strPrompt = _T( "Loading Collection  " );
	BSTR bstrPrompt = strPrompt.AllocSysString();
	m_pComponent->m_pIFramework->StartProgressBar(0, 100, bstrPrompt, &theApp.m_hKeyProgressBar);
	m_pComponent->m_pIFramework->SetProgressBarPos( theApp.m_hKeyProgressBar, 0 );
	theApp.m_dwProgressBarIncrement = pckMain->cksize / 100;
	theApp.m_dwProgressBarIncrement = theApp.m_dwProgressBarIncrement == 0 ? 1 : theApp.m_dwProgressBarIncrement;
	theApp.m_dwProgressBarTotalBytes = pckMain->cksize; 
	theApp.m_dwProgressBarRemainingBytes = pckMain->cksize; 

	CSystemConfiguration* pConfig = NULL;
	
	IStream* pIStream;
	MMCKINFO ck;
	DWORD cb;
	DWORD cSize;
    HRESULT hr = S_OK;

	ASSERT(pIRiffStream != NULL);
	ASSERT(pckMain != NULL);
	ASSERT(m_pComponent != NULL);

    pIStream = pIRiffStream->GetStream();
	
	ASSERT(pIStream != NULL);

	ck.ckid = 0;
	ck.fccType = 0;
    while(pIRiffStream->Descend( &ck, pckMain, 0 ) == 0)
	{
		switch(ck.ckid)
		{
			case FOURCC_EDIT:
			case FOURCC_MSYN:
				break;
			case FOURCC_COLH :
            {
				if(ck.cksize < sizeof(DLSHEADER))
				{
					cSize = ck.cksize;
				}
				else
				{
					cSize = sizeof(DLSHEADER);
				}
				hr = pIStream->Read( &m_rDLSHeader, cSize, &cb );
			}
			
			if( FAILED(hr) || cb != cSize )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			break;
		
			case FOURCC_VERS :
			{
				if( ck.cksize < sizeof(DLSVERSION) )
				{
					cSize = ck.cksize;
				}
				else
				{
					cSize = sizeof(DLSVERSION);
				}

				hr = pIStream->Read( &m_rVersion, cSize, &cb );
			}
			
			if( FAILED(hr) || cb != cSize )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
			
			break;

			case FOURCC_DLID:
				hr = pIStream->Read( &m_Guid, sizeof(GUID), &cb );
				if( FAILED(hr) || cb != sizeof(GUID))
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
				break;

			case FOURCC_LIST:
			switch (ck.fccType)
			{
				case FOURCC_LINS :
				{
					DWORD dwOrigRemainingBytes = theApp.m_dwProgressBarRemainingBytes;
					if(FAILED(hr = m_Instruments.Load(pIRiffStream,&ck)))
					{
						goto ON_ERROR;
					}
					theApp.m_dwProgressBarRemainingBytes = dwOrigRemainingBytes;	
					break;
				}

				case mmioFOURCC('I','N','F','O') :
					m_Info.Load(pIRiffStream,&ck);
					break;

				case FOURCC_WVPL :
                case mmioFOURCC('d','w','p','l') :
				{
					DWORD dwOrigRemainingBytes = theApp.m_dwProgressBarRemainingBytes;
					if(FAILED(m_Waves.Load(pIRiffStream,&ck)))
					{
						// We will refuse to load the collection if every single wave failed to load.
						// Could happen with collections that have waves compressed in unknown formats...
						if(m_Waves.GetCount() == 0)
						{
							// Could not load the waves in this collection. They may be using an unknown compression.
							// Please make sure the proper codec is installed on your computer, or uncompress the
							// waves in this DLS collection before loading it into DirectMusic Producer. 
							AfxMessageBox(IDS_ERR_COLLECTION_NO_WAVES);
							hr = E_FAIL;
							goto ON_ERROR;
						}

						// Could not load some waves in this collection. They will be removed from the designtime file.
						// These waves may have been using an unknown compression. Please make sure the proper codec
						// is installed on your computer, or uncompress these waves before loading them into DirectMusic Producer.
						AfxMessageBox(IDS_ERR_COLLECTION_WAVE_LOAD);
					}
					theApp.m_dwProgressBarRemainingBytes = dwOrigRemainingBytes;	
					break;
				}

                default:
                    m_UnknownChunk.Load(pIRiffStream, &ck, TRUE);
                    break;
			}
			break;

            case FOURCC_PTBL:
                break;

            default:
                m_UnknownChunk.Load(pIRiffStream, &ck, FALSE);
                break;
        }

		theApp.m_dwProgressBarRemainingBytes -= ck.cksize;
		m_pComponent->m_pIFramework->SetProgressBarPos( theApp.m_hKeyProgressBar,
					(theApp.m_dwProgressBarTotalBytes - theApp.m_dwProgressBarRemainingBytes) / theApp.m_dwProgressBarIncrement );

        pIRiffStream->Ascend( &ck, 0 );
    }

	ResolveConnections();

	// Setup for the current config
	pConfig = m_pComponent->GetCurrentConfig();
	if(pConfig)
	{
		OnConditionConfigChanged(pConfig, false);
	}

	// Check to make sure that the Framework has not set this flag to
	// FALSE. Which means that the collection will be loaded later on 
	// FileOpen().
	if (TRUE == m_pComponent->IsCollectionDownloadOK())
	{
		if(FAILED(Download()))
		{
			// Don't throw up the error-message if the MIDI button is toggled off
			if(m_pComponent->FindCollectionGUID(m_Guid) == NULL && m_bNoMIDIPort == FALSE)
			{
				AfxMessageBox(IDS_ERR_DOWNLOAD);
			}

			m_pComponent->AddToFailedCollectionGUIDs(m_Guid);
			hr = S_FALSE;
		}

		if(FAILED(hr) == FALSE)
		{
			m_pComponent->AddToCollectionList(this);
			m_Waves.RegisterWavesWithTransport();
		}
	}


ON_ERROR:
	pIStream->Release();
	m_pComponent->m_pIFramework->EndProgressBar( theApp.m_hKeyProgressBar );
	theApp.m_hKeyProgressBar = NULL;
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CCollection::SampleCount

DWORD CCollection::SampleCount()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);		
    CWaveNode* pWaveNode = m_Waves.GetHead();
    DWORD dwCount = 0;
    for( ; pWaveNode != NULL; pWaveNode = pWaveNode->GetNext())
    {
		CWave* pWave = pWaveNode->GetWave();
		ASSERT(pWave);
		
        dwCount += pWave->GetWaveLength();
    }
    return dwCount;
}

/////////////////////////////////////////////////////////////////////////////
// CCollection::ArticulationCount

DWORD CCollection::ArticulationCount()
{
    CInstrument *pInstrument = m_Instruments.GetHead();
    DWORD dwCount = 0;
    for ( ; pInstrument != NULL; pInstrument = pInstrument->GetNext())
    {
		CRegion *pRegion = pInstrument->m_Regions.GetHead();
		for ( ; pRegion != NULL; pRegion = pRegion->GetNext())
		{
			if (pRegion->m_pArticulation)
			{
				dwCount++;
			}
		}
		
		dwCount += pInstrument->GetArticulationCount();
    }
    return dwCount;
}

/////////////////////////////////////////////////////////////////////////////
// CCollection::RegionCount

DWORD CCollection::RegionCount()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);		    
	CInstrument *pInstrument = m_Instruments.GetHead();
    DWORD dwCount = 0;
    for ( ; pInstrument != NULL; pInstrument = pInstrument->GetNext())
    {
        dwCount += pInstrument->m_Regions.GetCount();
    }
    return dwCount;
}

/////////////////////////////////////////////////////////////////////////////
// CCollection::InstrumentCount

DWORD CCollection::InstrumentCount()

{
    CInstrument *pInstrument = m_Instruments.GetHead();
    DWORD dwCount = 0;
    for (;pInstrument != NULL;pInstrument = pInstrument->GetNext())
    {
        dwCount++;
    }
    return dwCount;
}

/////////////////////////////////////////////////////////////////////////////
// CCollection::WaveCount

DWORD CCollection::WaveCount()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);		    
	
	CWaveNode* pWaveNode = m_Waves.GetHead();
    DWORD dwCount = 0;
    for (;pWaveNode != NULL; pWaveNode = pWaveNode->GetNext())
    {
        dwCount++;
    }
    return dwCount;
}

/////////////////////////////////////////////////////////////////////////////
// CCollection::WaveNodeCount

DWORD CCollection::WaveNodeCount()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return m_Waves.GetCount();
}

/////////////////////////////////////////////////////////////////////////////
// CCollection IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CCollection IDMUSProdPropPageObject::GetData

HRESULT CCollection::GetData(void** ppData)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(ppData);

	*ppData = this;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CCollection IDMUSProdPropPageObject::SetData

HRESULT CCollection::SetData(void* pData)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pData);

	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CCollection IDMUSProdPropPageObject::OnRemoveFromPageManager

HRESULT CCollection::OnRemoveFromPageManager(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CCollection IDMUSProdPropPageObject::OnShowProperties

HRESULT CCollection::OnShowProperties(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = E_FAIL;

	ASSERT(m_pComponent != NULL);
	ASSERT(m_pComponent->m_pIFramework != NULL);

	// Show the Collection properties
	IDMUSProdPropSheet* pIPropSheet;

	hr = m_pComponent->m_pIFramework->QueryInterface(IID_IDMUSProdPropSheet, (void**)&pIPropSheet);
	if(SUCCEEDED(hr))
	{
		int nLastPage = CCollectionPropPgMgr::m_dwLastSelPage;
		hr = pIPropSheet->SetPageManager(m_pCollectionPropPgMgr);
		
		if(SUCCEEDED(hr))
		{
			m_pCollectionPropPgMgr->SetObject(this);
			pIPropSheet->SetActivePage(short(nLastPage)); 
		}

		pIPropSheet->Show(TRUE);
		pIPropSheet->Release();
	}

	return hr;
}

bool CCollection::IsWaveReferenced(CWave *pWave, IDMUSProdNode** ppReferenceNode)
{
	CInstrument*	pInstr;
	CRegion*		pRegion;

	pInstr = m_Instruments.GetHead();
	while (pInstr)
		{
		pRegion = pInstr->m_Regions.GetHead();
		while ( pRegion )
			{
			if (pWave->IsReferencedBy(pRegion->m_pWave))
				{
				*ppReferenceNode = pInstr;
				return true;
				}

			pRegion = pRegion->GetNext();
			}
		pInstr = pInstr->GetNext();
		}
	return false;
}

void CCollection::ValidateInstrumentPatches()
{
	CInstrument*	pInstrument = m_Instruments.GetHead();
	while (pInstrument)
	{
		if( !pInstrument->ValidatePatch() )
		{
			m_pComponent->NotifyDummyInstruments();
			pInstrument->UpdateInstrument();
			pInstrument->UpdatePatch();
		}
		pInstrument = pInstrument->GetNext();
	}
	m_fPatchResolution = 0;
}

BOOL CCollection::VerifyDLS()
{
    CInstrument* pInstrument = m_Instruments.GetHead();
    for (;pInstrument;pInstrument = pInstrument->GetNext())
    {
        CInstrument* pScan = pInstrument->GetNext();
        
		for (;pScan;pScan = pScan->GetNext())
        {
            if (pInstrument->m_rInstHeader.Locale.ulInstrument == pScan->m_rInstHeader.Locale.ulInstrument)
            {
                if (pInstrument->m_rInstHeader.Locale.ulBank == pScan->m_rInstHeader.Locale.ulBank)
                {
					CString sErrorMessage;
					CString sContinuePrompt;
					sContinuePrompt.LoadString(IDS_CONTINUE_VERIFYDLS);
					sErrorMessage.Format(IDS_ERR_INST_SAME_ADDRESS,	pInstrument->m_csName,pScan->m_csName);
					sErrorMessage = m_Info.m_csName + ":" + sErrorMessage + "\n" + sContinuePrompt;
					if(IDCANCEL == AfxMessageBox(sErrorMessage, MB_OKCANCEL | MB_ICONEXCLAMATION,0))
						return FALSE;
                }
            }
        }

        if (!pInstrument->VerifyDLS()) 
			return FALSE;
    }

	return TRUE;
}

CCollectionInstruments* CCollection::GetInstruments()
{
	return &m_Instruments;
}

HRESULT CCollection::GetObjectDescriptor( void* pObjectDesc )
{
	// Make sure method was passed a valid DMUS_OBJECTDESC pointer
	if( pObjectDesc == NULL ) 
	{
		ASSERT( FALSE );
		return E_POINTER;
	}

	DMUS_OBJECTDESC *pDMObjectDesc = (DMUS_OBJECTDESC *)pObjectDesc;

	if( pDMObjectDesc->dwSize == 0 ) 
	{
		ASSERT( FALSE );
		return E_INVALIDARG;
	}

	// Initialize DMUS_OBJECTDESC structure
	DWORD dwOrigSize = pDMObjectDesc->dwSize;
	memset( pDMObjectDesc, 0, dwOrigSize );
	pDMObjectDesc->dwSize = dwOrigSize;

	// Set values in DMUS_OBJECTDESC structure
	pDMObjectDesc->dwValidData = (DMUS_OBJ_OBJECT | DMUS_OBJ_CLASS | DMUS_OBJ_VERSION | DMUS_OBJ_NAME );
	
	memcpy( &pDMObjectDesc->guidObject, &m_Guid, sizeof(GUID) );
	memcpy( &pDMObjectDesc->guidClass, &CLSID_DirectMusicCollection, sizeof(CLSID) );
	pDMObjectDesc->vVersion.dwVersionMS = m_rVersion.dwVersionMS;
	pDMObjectDesc->vVersion.dwVersionLS = m_rVersion.dwVersionLS;
	MultiByteToWideChar( CP_ACP, 0, m_Info.m_csName, -1, pDMObjectDesc->wszName, DMUS_MAX_NAME );

	return S_OK;
}


HRESULT CCollection::GetListOfConditions(CPtrList* pConditionList)
{
	ASSERT(pConditionList);
	if(pConditionList == NULL)
		return E_POINTER;

	
	CInstrument* pInstrument = m_Instruments.GetHead();
	while(pInstrument)
	{
		if(FAILED(pInstrument->GetListOfConditions(pConditionList)))
		{
			return E_FAIL;
		}

		pInstrument = pInstrument->GetNext();
	}

	return S_OK;
}

void CCollection::OnConditionConfigChanged(CSystemConfiguration* pCurrentConfig, bool bRefreshNode)
{
	ASSERT(pCurrentConfig);
	if(pCurrentConfig == NULL)
	{
		return;
	}

	// Notify all instruments
	CInstrument* pInstrument = m_Instruments.GetHead();
	while(pInstrument)
	{
		pInstrument->OnConditionConfigChanged(pCurrentConfig, bRefreshNode);
		pInstrument = pInstrument->GetNext();
	}
}

void CCollection::SetNodeFileName(CString sFileName)
{
	ASSERT(sFileName.IsEmpty() == FALSE);
	m_sFileName = sFileName;
}

CString	CCollection::GetNodeFileName()
{
	// If the collection name is empty then goto the framework to get the owner's filename
	// This might be a wavenode in an embedded collection
	if(m_sFileName.IsEmpty())
	{
		ASSERT(m_pComponent);
		ASSERT(m_pComponent->m_pIFramework);
		IDMUSProdFramework* pIFramework = m_pComponent->m_pIFramework;
		BSTR bstrFileName;
		if(FAILED(pIFramework->GetNodeFileName(this, &bstrFileName)))
		{
			return "";
		}
		
		m_sFileName = bstrFileName;
		::SysFreeString(bstrFileName);
	}
	
	return m_sFileName;
}

HRESULT CCollection::FindWaveNodeByGUID(GUID guidNode, CWaveNode** ppWaveNode)
{
	ASSERT(ppWaveNode);
	if(ppWaveNode == NULL)
	{
		return E_POINTER;
	}

	CWaveNode* pNode = m_Waves.GetHead();
	while(pNode)
	{
		GUID guid = pNode->GetGUID();

		if(IsEqualGUID(guid, guidNode))
		{
			*ppWaveNode = pNode;
			return S_OK;
		}

		pNode = pNode->GetNext();
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CCollection::GetGUID

void CCollection::GetGUID( GUID* pguidCollection )
{
	if( pguidCollection )
	{
		*pguidCollection = m_Guid;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCollection::SetGUID

void CCollection::SetGUID( GUID guidCollection )
{
	m_Guid = guidCollection;
	SetDirtyFlag();

	// Notify connected nodes that Collection GUID has changed
	m_pComponent->m_pIFramework->NotifyNodes( this, DOCROOT_GuidChange, NULL );
}


//////////////////////////////////////////////////////////////////////////////
// CCollection::CreateDMCollection
HRESULT CCollection::CreateDMCollection()
{
	ASSERT(m_pIDMCollection == NULL);
	if(m_pIDMCollection)
	{
		return E_UNEXPECTED;
	}

	if(FAILED(::CoCreateInstance(CLSID_DirectMusicCollection, NULL, CLSCTX_INPROC, IID_IDirectMusicCollection, (void**)&m_pIDMCollection)))
	{
		return E_FAIL;
	}

	// Just making sure....
	ASSERT(m_pIDMCollection);
	if(FAILED(SyncDMCollection()))
	{
		return E_FAIL;
	}
	
	return S_OK;
}


HRESULT CCollection::SyncDMCollection()
{
	ASSERT(m_pIDMCollection);
	if(m_pIDMCollection == NULL)
	{
		return E_FAIL;
	}
	
	// No need to load the DirectMusicObject if we're already downloaded
	if(m_pComponent->IsAutoDownloadOkay() == TRUE)
	{
		return S_FALSE;
	}

	IPersistStream* pIPersistStream = NULL;
	if(FAILED(m_pIDMCollection->QueryInterface(IID_IPersistStream, (void**)&pIPersistStream)))
	{
		m_pIDMCollection->Release();
		m_pIDMCollection = NULL;
		return E_FAIL;
	}

	IStream* pIStream = NULL;
	if(FAILED(m_pComponent->m_pIFramework->AllocMemoryStream(FT_RUNTIME, GUID_CurrentVersion, &pIStream)))
	{
		pIPersistStream->Release();
		m_pIDMCollection->Release();
		m_pIDMCollection = NULL;
		return E_FAIL;
	}

	if(FAILED(Save(pIStream, FALSE)))
	{
		pIStream->Release();
		pIPersistStream->Release();
		m_pIDMCollection->Release();
		m_pIDMCollection = NULL;
	}

	// Seek back to the start of the stream
	StreamSeek( pIStream, 0, STREAM_SEEK_SET );


	if(FAILED(pIPersistStream->Load(pIStream)))
	{
		pIStream->Release();
		pIPersistStream->Release();
		m_pIDMCollection->Release();
		m_pIDMCollection = NULL;

		return E_FAIL;
	}

	pIStream->Release();
	pIPersistStream->Release();

	return S_OK;
}


void CCollection::ReleaseDMCollection()
{
	if(m_pIDMCollection)
	{
		m_pIDMCollection->Release();
		m_pIDMCollection = NULL;
	}
}

void CCollection::SetDirtyFlag() 
{
	m_bIsDirty = true;
}
