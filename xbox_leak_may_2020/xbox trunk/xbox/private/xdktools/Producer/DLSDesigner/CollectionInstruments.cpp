// CollectionInstruments.cpp : implementation file
//

#include "stdafx.h"
#include "DlsDefsPlus.h"
#include "DLSDesignerDLL.h"
#include "DLSDesigner.h"
#include "Collection.h"
#include "Instrument.h"
#include "Region.h"
#include "Articulation.h"
#include "WaveNode.h"
#include "wave.h"
#include "MonoWave.h"
#include "StereoWave.h"

// Comment out the below line to make the IDM_NEW_INSTRUMENT command dump the instrument list
// to C:\collection.txt before it inserts a new instrument
// #define DUMP_INSTRUMENTS_ON_NEW_INSTRUMENT 1

#ifdef DUMP_INSTRUMENTS_ON_NEW_INSTRUMENT
#include <fstream.h>
#endif // DUMP_INSTRUMENTS_ON_NEW_INSTRUMENT

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments constructor/destructor

CCollectionInstruments::CCollectionInstruments() :
m_dwRef(0),
m_pIRootNode(NULL),
m_pIParentNode(NULL),
m_pDLSComponent(NULL),
m_pCollection(NULL)
{
	AddRef();
}

CCollectionInstruments::~CCollectionInstruments()
{
	while(!IsEmpty())
	{
        CInstrument *pInstrument = RemoveHead();
        pInstrument->Release();
	}
}

void CCollectionInstruments::AddTail(CInstrument *pINode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pINode);

	AList::AddTail((AListItem *) pINode);
}

CInstrument *CCollectionInstruments::GetHead() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	return (CInstrument *) AList::GetHead();
}

CInstrument *CCollectionInstruments::RemoveHead() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	return (CInstrument *) AList::RemoveHead();
}

void CCollectionInstruments::Remove(CInstrument *pINode) 
{ 
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pINode);
	
	AList::Remove((AListItem *) pINode);
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments IUnknown implementation

HRESULT CCollectionInstruments::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	if( ::IsEqualIID(riid, IID_IDMUSProdNode)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CCollectionInstruments::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CCollectionInstruments::Release()
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
// CCollectionInstruments IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments IDMUSProdNode::GetNodeImageIndex

HRESULT CCollectionInstruments::GetNodeImageIndex(short* pnFirstImage)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pnFirstImage);

	ASSERT(m_pDLSComponent != NULL);

	return(m_pDLSComponent->GetFolderImageIndex(pnFirstImage));
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments IDMUSProdNode::GetComponent

HRESULT CCollectionInstruments::GetComponent(IDMUSProdComponent** ppIComponent)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(m_pDLSComponent != NULL);
	
	ASSERT(ppIComponent);
	
	return m_pDLSComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments IDMUSProdNode::GetRootNode

HRESULT CCollectionInstruments::GetDocRootNode(IDMUSProdNode** ppIRootNode)
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
// CCollectionInstruments IDMUSProdNode::SetRootNode

HRESULT CCollectionInstruments::SetDocRootNode(IDMUSProdNode* pIRootNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pIRootNode != NULL);

	m_pIRootNode = pIRootNode;
//	m_pIRootNode->AddRef();		intentionally missing

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments IDMUSProdNode::GetParentNode

HRESULT CCollectionInstruments::GetParentNode(IDMUSProdNode** ppIParentNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(m_pIParentNode != NULL);

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments IDMUSProdNode::SetParentNode

HRESULT CCollectionInstruments::SetParentNode(IDMUSProdNode* pIParentNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments IDMUSProdNode::GetNodeId

HRESULT CCollectionInstruments::GetNodeId(GUID* pguid)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pguid);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_InstrumentFolderNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments IDMUSProdNode::GetNodeName

HRESULT CCollectionInstruments::GetNodeName(BSTR* pbstrName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pbstrName);

	CString strName;
	TCHAR achBuffer[BUFFER_128];

	if(::LoadString(theApp.m_hInstance, IDS_INSTRUMENT_FOLDER_NAME, achBuffer, BUFFER_128))
	{
		strName = achBuffer;
	}

    *pbstrName = strName.AllocSysString();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments IDMUSProdNode::GetNodeNameMaxLength

HRESULT CCollectionInstruments::GetNodeNameMaxLength(short* pnMaxLength)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pnMaxLength);

	*pnMaxLength = -1;	// Can't rename a Instrument folder

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments IDMUSProdNode::ValidateNodeName

HRESULT CCollectionInstruments::ValidateNodeName(BSTR bstrName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Just free bstrName; can't rename Instruments folder
	::SysFreeString(bstrName);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments IDMUSProdNode::SetNodeName

HRESULT CCollectionInstruments::SetNodeName(BSTR bstrName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	// Just free bStrName; can't rename a Instrument folder	
	::SysFreeString( bstrName );

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments IDMUSProdNode::GetEditorClsId

HRESULT CCollectionInstruments::GetEditorClsId(CLSID* pClsId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't edit a Instrument folder
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments IDMUSProdNode::GetEditorTitle

HRESULT CCollectionInstruments::GetEditorTitle(BSTR* pbstrTitle)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	return E_NOTIMPL;	// Can't edit a Instrument folder
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments IDMUSProdNode::GetEditorWindow

HRESULT CCollectionInstruments::GetEditorWindow(HWND* hWndEditor)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	return E_NOTIMPL;	// Can't edit a Instrument folder
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments IDMUSProdNode::SetEditorWindow

HRESULT CCollectionInstruments::SetEditorWindow(HWND hWndEditor)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't edit a Instrument folder
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments IDMUSProdNode::UseOpenCloseImages

HRESULT CCollectionInstruments::UseOpenCloseImages(BOOL* pfUseOpenCloseImages)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pfUseOpenCloseImages);
	
	*pfUseOpenCloseImages = TRUE;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments IDMUSProdNode::GetRightClickMenuId

HRESULT CCollectionInstruments::GetRightClickMenuId(HINSTANCE* phInstance, UINT* pnMenuId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pnMenuId);
	ASSERT(phInstance);
	
	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_INSTRUMENTS_NODE_RMENU;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments IDMUSProdNode::OnRightClickMenuInit

HRESULT CCollectionInstruments::OnRightClickMenuInit(HMENU hMenu)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CMenu menu;
	
	if( menu.Attach(hMenu) )
	{
		// See if we need to disable the Paste menu item...
		IDataObject* pIDataObject;
		BOOL fWillSetReference;

		menu.EnableMenuItem( IDM_PASTE, (MF_GRAYED | MF_BYCOMMAND) );

		if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
		{
			if( CanPasteFromData( pIDataObject, &fWillSetReference ) == S_OK )
			{
				menu.EnableMenuItem( IDM_PASTE, (MF_ENABLED | MF_BYCOMMAND) );
			}
			
			pIDataObject->Release();
		}

		menu.Detach();
	}


	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments IDMUSProdNode::OnRightClickMenuSelect

HRESULT CCollectionInstruments::OnRightClickMenuSelect(long lCommandId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	HRESULT hr = E_FAIL;

	switch(lCommandId)
	{
		case IDM_NEW_INSTRUMENT:
		{
#ifdef DUMP_INSTRUMENTS_ON_NEW_INSTRUMENT
			if( m_pCollection )
			{
				ofstream oFile;
				oFile.open("c:\\collection.txt", ios::out );
				if( oFile.is_open() )
				{
					CInstrument *pInstrument = m_pCollection->m_Instruments.GetHead();
					while( pInstrument )
					{
						CString strName;
						pInstrument->GetName( strName );

						CString strInstrument;
						strInstrument.Format("%s\t%d\t%d\t%d\t%d\n", strName, (pInstrument->m_rInstHeader.Locale.ulBank >> 31), (pInstrument->m_rInstHeader.Locale.ulBank >> 8) & 0x7F, pInstrument->m_rInstHeader.Locale.ulBank & 0x7F, pInstrument->m_rInstHeader.Locale.ulInstrument);

						oFile << strInstrument;

						pInstrument = pInstrument->GetNext();
					}
					oFile.close();
				}
			}
#endif // DUMP_INSTRUMENTS_ON_NEW_INSTRUMENT

			hr = InsertChildNode(NULL);
			if(m_pCollection)
			{
				m_pCollection->ValidateInstrumentPatches();
				RefreshAllNodes();
			}
			break;
		}

		case IDM_PASTE:
		{
			IDataObject* pIDataObject = NULL;
			BOOL fWillSetReference = FALSE;
			if(SUCCEEDED(::OleGetClipboard( &pIDataObject)))
			{
				if(CanPasteFromData(pIDataObject, &fWillSetReference) == S_OK)
				{
					PasteFromData(pIDataObject);
				}
				
				RELEASE(pIDataObject);
			}
			break;
		}
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments IDMUSProdNode::DeleteChildNode

HRESULT CCollectionInstruments::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT( pIChildNode != NULL );
	ASSERT( m_pDLSComponent != NULL );
	ASSERT( m_pDLSComponent->m_pIFramework != NULL );

	// Remove node from Project Tree
	if( m_pDLSComponent->m_pIFramework->RemoveNode( pIChildNode, fPromptUser ) == S_FALSE )
	{
		return E_FAIL;
	}

	// Remove from list
	Remove((CInstrument *)pIChildNode);
	pIChildNode->Release();

	// Set flag so we know to save file 
	m_pCollection->SetDirtyFlag();

	m_pDLSComponent->m_pIFramework->NotifyNodes(m_pCollection, INSTRUMENT_Deleted, NULL);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments IDMUSProdNode::InsertChildNode

HRESULT CCollectionInstruments::InsertChildNode(IDMUSProdNode* pIChildNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pDLSComponent != NULL);
	ASSERT(m_pDLSComponent->m_pIFramework != NULL);
	ASSERT(m_pCollection != NULL);

	if(m_pCollection->m_Waves.GetHead() == NULL) // This is not an empty collection
	{
		AfxMessageBox(IDS_INVALID_INSERT_INSTRUMENT);
		return E_FAIL;
	}

	BYTE	bMSB(0), bLSB(0), bPatch(0);
	HRESULT	hr;
	
	if(pIChildNode == NULL)
	{
		pIChildNode = new CInstrument(m_pDLSComponent);
		
		if( pIChildNode == NULL )
		{
			return E_OUTOFMEMORY;
		}
		
		// Get a new unique patch
		m_pDLSComponent->GetNextValidPatch(0,&bMSB, &bLSB, &bPatch);
		((CInstrument*)pIChildNode)->m_rInstHeader.Locale.ulBank = MAKE_BANK(0, bMSB, bLSB);
		((CInstrument*)pIChildNode)->m_rInstHeader.Locale.ulInstrument = bPatch;

		// Create a single default region
		// It will be inserted in tree when the instrument gets inserted
		CRegion* pRegion = new CRegion(m_pDLSComponent);
		if(pRegion)
		{
			CWave* pFirstWave = m_pCollection->m_Waves.GetFirstWave();

            // Add a wave to the region. 
			pRegion->SetInitialWave(pFirstWave);

			// We're using the wave node now
			pFirstWave->GetNode()->AddRef();

			((CInstrument*)pIChildNode)->m_Regions.AddTail(pRegion);
			pRegion->m_pInstrument = ((CInstrument*)pIChildNode);
		}
		else
		{
			goto LDeleteChildAndFail;
		}
		//Download to synth
		POSITION position = m_pDLSComponent->m_lstDLSPortDownload.GetHeadPosition();
		while(position)
		{
			IDirectMusicPortDownload* pIDMPortDownload = m_pDLSComponent->m_lstDLSPortDownload.GetNext(position);
			ASSERT(pIDMPortDownload);

			hr = ((CInstrument *)pIChildNode)->DM_Init(pIDMPortDownload);
			if (SUCCEEDED(hr))
			{
				hr = ((CInstrument *)pIChildNode)->Download(pIDMPortDownload);
				if ( !SUCCEEDED(hr) )
				{
					AfxMessageBox(IDS_ERR_INIT_DOWNLOAD,MB_OK | MB_ICONEXCLAMATION);
				}
			}
			else
			{
				if(hr == E_OUTOFMEMORY)
				{
					delete ((CInstrument*)pIChildNode)->m_Regions.RemoveHead();
					goto LDeleteChildAndFail;
				}
				return hr;
			}
		}
	}
	else
	{
		pIChildNode->AddRef();
	}

	((CInstrument *)pIChildNode)->m_pCollection = m_pCollection;

	// add to Instrument list
	AddTail( (CInstrument *) pIChildNode );

	// Set root and parent node of ALL children
	theApp.SetNodePointers( pIChildNode, (IDMUSProdNode *)m_pCollection, (IDMUSProdNode *)this );

	// Add node to Project Tree
	if( !SUCCEEDED ( m_pDLSComponent->m_pIFramework->AddNode(pIChildNode, (IDMUSProdNode *)this) ) )
	{
		DeleteChildNode( pIChildNode, FALSE );
		return E_FAIL;
	}

	// Set flag so we know to save file 
	m_pCollection->SetDirtyFlag();
	
	return S_OK;

LDeleteChildAndFail:
	pIChildNode->Release();
	return E_OUTOFMEMORY;
}

////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments IDMUSProdNode::DeleteNode

HRESULT CCollectionInstruments::DeleteNode( BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////
//
//	CCollectionInstruments IDMUSProdNode::GetFirstChild
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionInstruments::GetFirstChild(IDMUSProdNode** ppIFirstChildNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(ppIFirstChildNode == NULL)
	{
		return E_POINTER;
	}

	*ppIFirstChildNode = NULL;

	IDMUSProdNode* pINode = (IDMUSProdNode*) GetHead();

	if(pINode)
	{
		pINode->AddRef();
		*ppIFirstChildNode = pINode;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CCollectionInstruments IDMUSProdNode::GetNextChild
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionInstruments::GetNextChild(IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode)
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

	IDMUSProdNode* pINode = (IDMUSProdNode*) ((CInstrument *)pIChildNode)->GetNext();

	if(pINode)
	{
		pINode->AddRef();
		*ppINextChildNode = pINode;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CCollectionInstruments IDMUSProdNode::GetNodeListInfo
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionInstruments::GetNodeListInfo(DMUSProdListInfo* pListInfo)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pListInfo);
	
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments IDMUSProdNode::OnNodeSelChanged

HRESULT CCollectionInstruments::OnNodeSelChanged( BOOL fSelected )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollectionInstruments IDMUSProdNode::CreateDataObject
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionInstruments::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollectionInstruments IDMUSProdNode::CanCut
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionInstruments::CanCut()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return S_OK;

}

/////////////////////////////////////////////////////////////////////////////
//
// CCollectionInstruments IDMUSProdNode::CanCopy
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionInstruments::CanCopy()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return S_OK;

}

/////////////////////////////////////////////////////////////////////////////
//
// CCollectionInstruments IDMUSProdNode::CanDelete
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionInstruments::CanDelete()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return S_OK;

}

/////////////////////////////////////////////////////////////////////////////
//
// CCollectionInstruments IDMUSProdNode::CanDeleteChildNode
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionInstruments::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return S_OK;

}

/////////////////////////////////////////////////////////////////////////////
//
// CCollectionInstruments IDMUSProdNode::CanPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionInstruments::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
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

	HRESULT hr = S_FALSE;
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_pDLSComponent->m_cfInstrument )) )
    {
        hr = S_OK;
    }

    pDataObject->Release();

    return hr;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollectionInstruments IDMUSProdNode::PasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionInstruments::PasteFromData( IDataObject* pIDataObject )
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

	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_pDLSComponent->m_cfInstrument) ) )
	{
		
		// Handle CF_WAVE format
		hr = PasteCF_INSTRUMENT( pDataObject, pIDataObject);
	}

    pDataObject->Release();

	// Check for patch conflicts after pasting the data
	if(SUCCEEDED(hr) && m_pCollection)
	{
		m_pCollection->ValidateInstrumentPatches();
		RefreshAllNodes();

		// This could be an instrument that's referenced by a band
		m_pDLSComponent->m_pIFramework->NotifyNodes(m_pCollection, INSTRUMENT_NameChange, NULL);

	}

    return hr;


}

/////////////////////////////////////////////////////////////////////////////
//
// CCollectionInstruments IDMUSProdNode::CanChildPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionInstruments::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
													   BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollectionInstruments IDMUSProdNode::ChildPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionInstruments::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return PasteFromData(pIDataObject);

}

/////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments IDMUSProdNode::GetObject

HRESULT CCollectionInstruments::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
//
// CCollectionInstruments PasteCF_INSTRUMENT
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionInstruments::PasteCF_INSTRUMENT( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject)
{
	ASSERT(m_pDLSComponent);
	if(m_pDLSComponent == NULL)
	{
		return E_UNEXPECTED;
	}

	IDMUSProdFramework* pIFramework = m_pDLSComponent->m_pIFramework;
	ASSERT(pIFramework);
	if(pIFramework == NULL)
	{
		return E_UNEXPECTED;
	}

	IStream* pStream = NULL;
	HRESULT hr = E_FAIL;
	bool bCopyWaves = true;
	LONG lOriginalWaveCount = m_pCollection->m_Waves.GetCount();
	
	
	UINT nUserConfirmation = AfxMessageBox(IDS_WAVE_COPY_CONFIRMATION, MB_YESNOCANCEL);    

	if(nUserConfirmation == IDNO)
	{
		if(lOriginalWaveCount == 0)
		{
			AfxMessageBox(IDS_NO_WAVES_WARNING);
			return E_FAIL;
		}

		bCopyWaves = false;
	}
	else if(nUserConfirmation == IDCANCEL)
	{
		return E_FAIL;
	}
		

    if (SUCCEEDED(pDataObject->AttemptRead(pIDataObject,  m_pDLSComponent->m_cfInstrument, &pStream )))
    {
		LARGE_INTEGER liTemp;

		// Seek to beginning of stream
		liTemp.QuadPart = 0;
		pStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

		// Create and load a new Instrument
		IDMUSProdNode* pINode = new CInstrument(m_pDLSComponent);
	    if( pINode == NULL )
	    {
	        return E_OUTOFMEMORY;
	    }
            
        IDMUSProdRIFFStream* pRiffStream = NULL;
        MMCKINFO ckMain;
        CInstrument* pInstrument = (CInstrument*)pINode;
        //alloc a riffstream
        hr = AllocRIFFStream(pStream, &pRiffStream);                
        if (SUCCEEDED(hr))
        {
            ckMain.fccType = FOURCC_INS;
            if( pRiffStream->Descend( &ckMain, NULL, MMIO_FINDLIST ) == 0 )
            {
                hr = pInstrument->Load(pRiffStream, &ckMain );
            }

			pRiffStream->Ascend( &ckMain, 0 );
		
			// Get a new unique patch
			BYTE bMSB(0), bLSB(0), bPatch(0);
			bool bDrums = (F_INSTRUMENT_DRUMS & pInstrument->m_rInstHeader.Locale.ulBank) != 0;
			m_pDLSComponent->GetNextValidPatch(bDrums,&bMSB, &bLSB, &bPatch);
			pInstrument->m_rInstHeader.Locale.ulBank = MAKE_BANK(bDrums, bMSB, bLSB);
			pInstrument->m_rInstHeader.Locale.ulInstrument = bPatch;

            // Add to the collection
            ((CInstrument*)pINode)->m_pCollection = m_pCollection;

            // add to the instrument list
            AddTail(pInstrument);
			
			if(bCopyWaves)
			{
				m_pCollection->m_Waves.PasteWaves(pRiffStream);
				m_pCollection->SetWaveIDs();

				CPtrList waveList;
				if(FAILED(m_pCollection->m_Waves.GetListOfMonoWaves(&waveList)))
				{
					pStream->Release();
					DeleteChildNode( pINode, FALSE );
					return E_OUTOFMEMORY;
				}

				CRegion* pRegion = pInstrument->m_Regions.GetHead();
				for ( ; pRegion != NULL; pRegion = pRegion->GetNext())
				{
					POSITION position = waveList.GetHeadPosition();
					while(position)
					{
						CWave* pWave = (CWave*) waveList.GetNext(position);
						ASSERT(pWave);

						// Set the wavelinks for copied regions and waves
						if(pWave->m_dwPreviousRegionLinkID == pRegion->m_rWaveLink.ulTableIndex)
						{
							// merge regions with same saved ID (different parts of mono waves)
							pRegion->MergeNextRegions(pWave);
						
							pRegion->m_rWaveLink.ulTableIndex = pWave->GetWaveID();
							pRegion->m_pWave = pWave;
							pWave->GetNode()->AddRef();
							pRegion->GetName();
							break;
						}
					}
				}
			}

			m_pCollection->ResolveConnections();

			// Set root and parent node of ALL children
            theApp.SetNodePointers( pINode, (IDMUSProdNode *)m_pCollection, (IDMUSProdNode *)this );

            // Add node to Project Tree
            if( !SUCCEEDED ( pIFramework->AddNode(pINode, (IDMUSProdNode *)this) ) )
            {
				pStream->Release();
	            DeleteChildNode( pINode, FALSE );
	            return E_FAIL;
            }

			pRiffStream->Release();

            // Set flag so we know to save file 
            m_pCollection->SetDirtyFlag();
			
			POSITION position = m_pDLSComponent->m_lstDLSPortDownload.GetHeadPosition();
			while(position)
			{
				IDirectMusicPortDownload* pIDMPortDownload = m_pDLSComponent->m_lstDLSPortDownload.GetNext(position);
				ASSERT(pIDMPortDownload);

				if ( pIDMPortDownload && SUCCEEDED(((CInstrument*)pINode)->DM_Init(pIDMPortDownload)) )
				{
					((CInstrument*)pINode)->Download(pIDMPortDownload);
				}
				else
				{
					pStream->Release();
					DeleteChildNode(pINode, FALSE);
					return E_FAIL;
				}

			}
            
			pStream->Release();
			pIFramework->SetSelectedNode(pINode);

        } // succeeded(hr)
		else
		{
			pStream->Release();
			pINode->Release();
			return E_FAIL;
		}
    }
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionInstruments::Load

HRESULT CCollectionInstruments::Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain)
{    
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	IStream* pIStream;
	MMCKINFO ck;
    HRESULT	hr = S_OK;

	ASSERT(m_pDLSComponent != NULL);
	ASSERT(pIRiffStream);
	ASSERT(pckMain);

    pIStream = pIRiffStream->GetStream();

	ASSERT(pIStream != NULL);

	ck.ckid = 0;
	ck.fccType = 0;
    while(pIRiffStream->Descend(&ck, pckMain, 0) == 0)
	{
		switch(ck.ckid) 
		{
			case FOURCC_LIST:
				switch(ck.fccType)
				{
					case FOURCC_INS :
						CInstrument *pInstrument = new CInstrument(m_pDLSComponent);
						if(pInstrument != NULL)
						{
							pInstrument->m_pCollection = m_pCollection;//for patch validation
							hr = pInstrument->Load(pIRiffStream, &ck);
							if( SUCCEEDED(hr) )
							{
								AddTail( pInstrument );
								pInstrument->m_pCollection = m_pCollection;
							}
							else
							{
								pInstrument->Release();
								pIStream->Release();
								return hr;
							}
						}
					break;
				}
			
			break;
		}

		theApp.m_dwProgressBarRemainingBytes -= ck.cksize;
		m_pDLSComponent->m_pIFramework->SetProgressBarPos( theApp.m_hKeyProgressBar,
					(theApp.m_dwProgressBarTotalBytes - theApp.m_dwProgressBarRemainingBytes) / theApp.m_dwProgressBarIncrement );

        pIRiffStream->Ascend(&ck, 0);
		ck.ckid = 0;
		ck.fccType = 0;
	}
	pIStream->Release();
	return hr;
}


void CCollectionInstruments::RefreshAllNodes()
{
	ASSERT(m_pDLSComponent->m_pIFramework);
	if(m_pDLSComponent->m_pIFramework == NULL)
		return;

	CInstrument* pInstrument = GetHead();
    for( ; pInstrument != NULL; pInstrument = pInstrument->GetNext())
	{
		m_pDLSComponent->m_pIFramework->RefreshNode(pInstrument);
		
	}
}

