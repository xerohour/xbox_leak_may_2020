// Instrument.cpp : implementation file
//

#include "stdafx.h"
#include "DlsDefsPlus.h"
#include "DLSDesignerDLL.h"
#include "DLSDesigner.h"
#include "Collection.h"
#include "Instrument.h"
#include "region.h"
#include "ArticulationList.h"
#include "Articulation.h"
#include "Wave.h"
#include "WaveNode.h"
#include "MonoWave.h"
#include "UndoMan.h"
#include "InstrumentFVEditor.h"
#include "InstrumentPropPgMgr.h"
#include "InstrumentPropPg.h"
#include "resource.h"
#include "JazzDataObject.h"
#include "InstrumentCtl.h"
#include "PatchConflictDlg.h"

#include <dmksctrl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CInstrument constructor/destructor

CInstrument::CInstrument(CDLSComponent* pComponent) : 

m_nMIDINote(60), 
m_pCollection(NULL),
m_pInstrumentCtrl(NULL),
m_pIRootNode(NULL),
m_pIParentNode(NULL),
m_hWndEditor(NULL),
m_pInstrumentPropPgMgr(NULL),
m_hStopEvent(0),
m_dwDLSize(0),
m_bNewFormat(FALSE),
m_pCurArticulation(NULL),
m_bAuditionMode(AUDITION_MULTIPLE)
{
	ASSERT(pComponent != NULL);

    m_dwRef = 0;
	AddRef();

	m_pComponent = pComponent;
	m_pComponent->AddRef();

	m_Regions.m_pDLSComponent = pComponent;
	m_Regions.m_pInstrument = this;

    m_rInstHeader.Locale.ulBank = 0;
    m_rInstHeader.Locale.ulInstrument = 0;
    m_rInstHeader.cRegions = 0;

	if(m_pInstrumentPropPgMgr == NULL)
	{
		m_pInstrumentPropPgMgr = new CInstrumentPropPgMgr();
	}
	else
	{
		m_pInstrumentPropPgMgr->AddRef();
	}

	GetName();
	memset(m_cMidiNoteOns,0,128 * sizeof(int));

	m_hStopEvent = ::CreateEvent(NULL, TRUE, FALSE, "Oneshot Timeout");

	// Create the Undo Manager
	m_pUndoMgr = new CUndoMan;
	::CoCreateGuid(&m_guidUndoStream);

	CreateDefaultGlobalArticulation();
}

CInstrument::~CInstrument()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	::CloseHandle(m_hStopEvent);

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
			if(m_pInstrumentPropPgMgr)
			{
				m_pInstrumentPropPgMgr->RemoveObject(this);
			}
		}
		pIPropSheet->Release();
	}

	if(m_pInstrumentPropPgMgr)
	{
		DWORD refCount = m_pInstrumentPropPgMgr->Release();
		if(refCount == 0)
			m_pInstrumentPropPgMgr = NULL;
	}

	while(!m_lstArticulationLists.IsEmpty())
	{
		CArticulationList* pArticList = (CArticulationList*) m_lstArticulationLists.RemoveHead();
		pArticList->Release();
	}

	if (!m_lstAllocatedBuffers.IsEmpty())
	{
		POSITION position = m_pComponent->m_lstDLSPortDownload.GetHeadPosition();
		while(position)
		{
			IDirectMusicPortDownload* pIDMPortDownload = m_pComponent->m_lstDLSPortDownload.GetNext(position);
			ASSERT(pIDMPortDownload);
			Unload(pIDMPortDownload);
		}
	}

	if(m_pComponent)
	{
		if(m_pComponent->Release() == 0)
			m_pComponent = NULL;
	}

	theApp.FlushClipboard( this );

	delete m_pUndoMgr;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument IUnknown implementation

HRESULT CInstrument::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CInstrument::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CInstrument::Release()
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
// CInstrument IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdNode::GetNodeImageIndex

HRESULT CInstrument::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pnFirstImage);
	
	ASSERT(m_pComponent != NULL);

	return(m_pComponent->GetInstrumentImageIndex(pnFirstImage));
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdNode::GetComponent

HRESULT CInstrument::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );

	return m_pComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdNode::GetRootNode

HRESULT CInstrument::GetDocRootNode( IDMUSProdNode** ppIRootNode )
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
// CInstrument IDMUSProdNode::SetRootNode

HRESULT CInstrument::SetDocRootNode( IDMUSProdNode* pIRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIRootNode != NULL );

	m_pIRootNode = pIRootNode;
//	m_pIRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdNode::GetParentNode

HRESULT CInstrument::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdNode::SetParentNode

HRESULT CInstrument::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdNode::GetNodeId

HRESULT CInstrument::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_InstrumentNode;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdNode::GetNodeName

HRESULT CInstrument::GetNodeName(BSTR* pbstrName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pbstrName);

    // Creates the instrument name and stores it in m_csName
	GetName();
	*pbstrName = m_csName.AllocSysString();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdNode::GetNodeNameMaxLength

HRESULT CInstrument::GetNodeNameMaxLength(short* pnMaxLength)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pnMaxLength);
	
	*pnMaxLength = BUFFER_256;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdNode::ValidateNodeName

HRESULT CInstrument::ValidateNodeName(BSTR bstrName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;

	strName = bstrName;
	
	::SysFreeString(bstrName);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdNode::SetNodeName

HRESULT CInstrument::SetNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName = bstrName;
	::SysFreeString( bstrName );

	m_Info.m_csName = strName;

	// Set flag so we know to save file 
 	m_pCollection->SetDirtyFlag();

	if(m_pInstrumentPropPgMgr)
	{
		IDMUSProdPropSheet* pIPropSheet;
		if( SUCCEEDED ( m_pComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			if( pIPropSheet->IsShowing() == S_OK && pIPropSheet->IsEqualPageManagerObject(this) == S_OK)
			{
				m_pInstrumentPropPgMgr->RefreshData();
				m_pInstrumentPropPgMgr->m_pInstrumentPage->OnSetActive();
			}
			
			RELEASE(pIPropSheet);
		}
	} 
	
	// Change the name in the transport
	if(m_pInstrumentCtrl)
			m_pInstrumentCtrl->SetTransportName();

	if(m_pComponent->m_pIFramework)
		m_pComponent->m_pIFramework->NotifyNodes(m_pCollection, INSTRUMENT_NameChange, NULL);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdNode::GetEditorClsId

HRESULT CInstrument::GetEditorClsId( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    *pClsId = CLSID_Instrument;
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdNode::GetEditorTitle

HRESULT CInstrument::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strTitle;

	strTitle = _T("Instrument: ");
	strTitle += m_csName;

    *pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdNode::GetEditorWindow

HRESULT CInstrument::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*hWndEditor = m_hWndEditor;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdNode::SetEditorWindow

HRESULT CInstrument::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_hWndEditor = hWndEditor;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdNode::UseOpenCloseImages

HRESULT CInstrument::UseOpenCloseImages(BOOL* pfUseOpenCloseImages)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pfUseOpenCloseImages);

	*pfUseOpenCloseImages = TRUE;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdNode::GetRightClickMenuId

HRESULT CInstrument::GetRightClickMenuId(HINSTANCE* phInstance, UINT* pnMenuId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pnMenuId);
	ASSERT(phInstance);

	if(phInstance == NULL
	|| pnMenuId == NULL)
	{
		return E_POINTER;
	}

	*phInstance = theApp.m_hInstance;
	*pnMenuId = IDM_INSTRUMENT_NODE_RMENU;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdNode::OnRightClickMenuInit

HRESULT CInstrument::OnRightClickMenuInit(HMENU hMenu)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    // If there is a global articulation, disable the insert articulation
    // menuitem
    /* (m_pArticulation)
    {
        CMenu menu;
 	    if( menu.Attach(hMenu) )
        {
			menu.EnableMenuItem( IDM_INSERT_ART, (MF_GRAYED | MF_BYCOMMAND) );
        }
        menu.Detach();
    }*/
   
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdNode::OnRightClickMenuSelect

HRESULT CInstrument::OnRightClickMenuSelect(long lCommandId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pComponent != NULL);
	ASSERT(m_pComponent->m_pIFramework != NULL);

	HRESULT hr = E_FAIL;
	IDMUSProdNode* pIParentNode;
	
	switch( lCommandId )
	{
		case IDM_OPEN:
		{
			if(SUCCEEDED(m_pComponent->m_pIFramework->OpenEditor(this)))
			{
				hr = S_OK;
			}
			break;
		}

		case IDM_INSERT_ART_LIST:
		{
			SaveStateForUndo(IDS_UNDO_INSERT_ARTLIST);
			InsertChildNode(NULL);			
			hr = S_OK;
			break;
		}

		case IDM_CUT:
		{
			if(CanCut() == S_OK)
			{
				IDataObject* pIDataObject = NULL;
				if(SUCCEEDED(CreateDataObject(&pIDataObject)))
				{
					if(theApp.PutDataInClipboard( pIDataObject, this))
					{
						DeleteNode(FALSE);
						if(m_pComponent->m_pIFramework)
							m_pComponent->m_pIFramework->NotifyNodes(m_pCollection, INSTRUMENT_Deleted, NULL);
						hr = S_OK;
					}
					pIDataObject->Release();
					pIDataObject = NULL;
				}
			}
			break;
		}

		case IDM_COPY:
		{
			if(CanCopy() == S_OK)
			{
				IDataObject* pIDataObject = NULL;
				if(SUCCEEDED(CreateDataObject(&pIDataObject)))
				{
					if(theApp.PutDataInClipboard( pIDataObject, this))
					{
						hr = S_OK;
					}
					pIDataObject->Release();
					pIDataObject = NULL;
				}
			}
			break;
		}

		case IDM_DELETE:
		{
			if(SUCCEEDED(GetParentNode(&pIParentNode)))
			{
				if(SUCCEEDED(pIParentNode->DeleteChildNode((IDMUSProdNode *)this, TRUE)))
				{
					hr = S_OK;				
				}
				pIParentNode->Release();
			}
			break;
		}

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
	}
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdNode::DeleteChildNode

HRESULT CInstrument::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pIChildNode != NULL);
	ASSERT(m_pComponent != NULL);
	ASSERT(m_pComponent->m_pIFramework != NULL);

	bool bIsSafeToDelete = true;

	UINT nGlobalArts = GetArticulationCount();

	CRegion* pRegion = m_Regions.GetHead();
	if(nGlobalArts == 0)
	{
		// Before we can delete the Global Articulation we want to make sure that
		// each Region has its own articulation
		ASSERT(pRegion);
		for(; pRegion && bIsSafeToDelete; pRegion = pRegion->GetNext())
		{
			if(pRegion->GetArticulationCount() == 0)
			{
				bIsSafeToDelete = false;
			}
		}
	}

	// Remove node from Project Tree
	if(!bIsSafeToDelete || m_pComponent->m_pIFramework->RemoveNode(pIChildNode, fPromptUser) == S_FALSE)
	{
		return E_FAIL;
	}

	// Remove it from the list of articulation lists 
	POSITION position = m_lstArticulationLists.Find(dynamic_cast<CArticulationList*>(pIChildNode));
	if(position)
	{
		m_lstArticulationLists.RemoveAt(position);
	}

	pIChildNode->Release();
	
	m_pCurArticulation = NULL;
	SetFirstGlobaArtAsCurrent();

	// Set flag so we know to save file 
	m_pCollection->SetDirtyFlag();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdNode::InsertChildNode

HRESULT CInstrument::InsertChildNode(IDMUSProdNode* pIChildNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pComponent != NULL);
	ASSERT(m_pComponent->m_pIFramework != NULL);
	
	if(pIChildNode == NULL)
	{
		pIChildNode = new CArticulationList(m_pComponent, this);

		if(pIChildNode == NULL)
		{
			return E_OUTOFMEMORY;
		}
	}
	else
	{
		pIChildNode->AddRef();
	}

	m_lstArticulationLists.AddTail((CArticulationList*)pIChildNode);
	m_pCurArticulation = ((CArticulationList*)pIChildNode)->GetHead();
	
	// Set root and parent node of ALL children
	theApp.SetNodePointers(pIChildNode, (IDMUSProdNode *)m_pCollection, (IDMUSProdNode *)this);

	// Add node to Project Tree
	if(!SUCCEEDED(m_pComponent->m_pIFramework->AddNode(pIChildNode, (IDMUSProdNode *)this)))
	{
		DeleteChildNode(pIChildNode, FALSE);
		return E_FAIL;
	}

	// Set flag to indicate the DLS collection now needs to be saved
	m_pCollection->SetDirtyFlag();

	return S_OK;
}

////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdNode::DeleteNode

HRESULT CInstrument::DeleteNode( BOOL fPromptUser )
{
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
	return E_FAIL;
}

//////////////////////////////////////////////////////////////////////
//
//	CInstrument IDMUSProdNode::GetFirstChild
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrument::GetFirstChild(IDMUSProdNode** ppIFirstChildNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(ppIFirstChildNode == NULL)
	{
		return E_POINTER;
	}

	*ppIFirstChildNode = NULL;

	IDMUSProdNode* pINode = static_cast<IDMUSProdNode*>(&m_Regions);

	if(pINode)
	{
		pINode->AddRef();
		*ppIFirstChildNode = pINode;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CInstrument IDMUSProdNode::GetNextChild
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrument::GetNextChild(IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode)
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

	IDMUSProdNode* pINode = NULL;

	POSITION position = NULL;
	CArticulationList* pArticList = dynamic_cast<CArticulationList*>(pIChildNode);
	if(pArticList)
	{
		position = m_lstArticulationLists.Find(pArticList);
	}
	
	if(pIChildNode == dynamic_cast<IDMUSProdNode*>(&m_Regions))
	{
		if(!m_lstArticulationLists.IsEmpty())
		{
			pINode = dynamic_cast<IDMUSProdNode*>(m_lstArticulationLists.GetHead());
		}
	}
	else if(position)		
	{
		CArticulationList* pPassedArticList = (CArticulationList*) m_lstArticulationLists.GetNext(position);
		if(position)
		{
			pArticList = (CArticulationList*) m_lstArticulationLists.GetNext(position);
			pINode = dynamic_cast<IDMUSProdNode*>(pArticList);
		}
	}

	if(pINode)
	{
		pINode->AddRef();
		*ppINextChildNode = pINode;
	}
	else
	{
		*ppINextChildNode = NULL;
	}
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CInstrument IDMUSProdNode::GetNodeListInfo
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrument::GetNodeListInfo(DMUSProdListInfo* pListInfo)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pListInfo);
	
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdNode::OnNodeSelChanged

HRESULT CInstrument::OnNodeSelChanged(BOOL fSelected)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	HRESULT hr = S_OK;
	
	TurnOffMidiNotes();
	if(m_pInstrumentCtrl)
	{
		m_pInstrumentCtrl->TurnOffMidiNotes();
	}

	if(fSelected)
	{
		ASSERT(m_pComponent != NULL);
		ASSERT(m_pComponent->m_pIFramework != NULL);
		if(m_pInstrumentCtrl)
		{
			m_pInstrumentCtrl->SetTransportName();
			m_pComponent->m_pIConductor->SetActiveTransport(this, BS_PLAY_ENABLED | BS_NO_AUTO_UPDATE);
			UpdatePatch();
		}
	}	
	/*else
	{
		TRACE("Instrument Control: OnNodeSel - NOTSELECTED\n");
		
		m_pComponent->m_pIConductor->TransportStopped(this);
		m_pComponent->m_pIConductor->SetBtnStates(this, BS_NO_AUTO_UPDATE);
	}*/
	return hr; 
}



/////////////////////////////////////////////////////////////////////////////
//
// CInstrument IDMUSProdNode::CreateDataObject
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrument::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

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
	// Save Instrument into stream
	HRESULT hr = E_FAIL;
	if( SUCCEEDED ( m_pComponent->m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		//if( SUCCEEDED (Save( pIStream, FALSE ) ) )
		IDMUSProdRIFFStream* pRiffStream = NULL;

		hr = AllocRIFFStream(pIStream, &pRiffStream);                
		if (SUCCEEDED(hr))
		{
			hr = Save(pRiffStream, TRUE);  //fullsave is true.
		}

		// Place CF_INSTRUMENT into CDllJazzDataObject
		if( SUCCEEDED ( pDataObject->AddClipFormat(m_pComponent->m_cfInstrument, pIStream ) ) )
		{
			hr = S_OK;
		}
	
		// We also need to copy all the waves...
		// m_bDragCopy flag helps in not copying multiple copies of a wave referenced by multiple regions
		CRegion* pRegion = m_Regions.GetHead();
		for ( ; pRegion; pRegion = pRegion->GetNext())
		{
			if(pRegion->m_pWave == NULL)
			{
				continue;
			}

			CWaveNode* pWaveNode = pRegion->GetWaveNode();
			ASSERT(pWaveNode);
			if(pWaveNode && pWaveNode->GetDragCopy() == false)
			{
				if(FAILED(pWaveNode->WriteDragChunkToStream(pRiffStream)))
				{
					pIStream->Release();
					pRiffStream->Release();
					return E_FAIL;
				}
			}
		}

		// Clear the dragCopy flags now...
		pRegion = m_Regions.GetHead();
		for ( ; pRegion; pRegion = pRegion->GetNext())
		{
			CWaveNode* pWaveNode = pRegion->GetWaveNode();
			ASSERT(pWaveNode);
			if(pWaveNode)
			{
				pWaveNode->SetDragCopy(false);
			}
		}


		// Place CF_WAVE into CDllJazzDataObject
		if( SUCCEEDED ( pDataObject->AddClipFormat(m_pComponent->m_cfWave, pIStream ) ) )
		{
			hr = S_OK;
		}

		if (pRiffStream)
		{
			pRiffStream->Release();
		}
		


		pIStream->Release();
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
// CInstrument IDMUSProdNode::CanCut
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrument::CanCut()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// CInstrument IDMUSProdNode::CanCopy
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrument::CanCopy()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// CInstrument IDMUSProdNode::CanDelete
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrument::CanDelete()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// CInstrument IDMUSProdNode::CanDeleteChildNode
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrument::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CInstrument IDMUSProdNode::CanPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrument::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
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

	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_pComponent->m_cfInstrument)))
    {
        hr = S_OK;
    }

    pDataObject->Release();

    return hr;

}

/////////////////////////////////////////////////////////////////////////////
//
// CInstrument IDMUSProdNode::PasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrument::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Pass it to the parent to paste.
    ASSERT(m_pIParentNode != NULL);
	return m_pIParentNode->ChildPasteFromData( pIDataObject, this );

}

/////////////////////////////////////////////////////////////////////////////
//
// CInstrument IDMUSProdNode::CanChildPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrument::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
										    BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;

}

/////////////////////////////////////////////////////////////////////////////
//
// CInstrument IDMUSProdNode::ChildPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrument::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdNode::GetObject

HRESULT CInstrument::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdPropPageObject::GetData

HRESULT CInstrument::GetData(void** ppData)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(ppData);
	*ppData = this;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdPropPageObject::SetData

HRESULT CInstrument::SetData(void* pData)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CCollection IDMUSProdPropPageObject::OnRemoveFromPageManager

HRESULT CInstrument::OnRemoveFromPageManager(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CCollection IDMUSProdPropPageObject::OnShowProperties

HRESULT CInstrument::OnShowProperties(void)
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
		hr = pIPropSheet->SetPageManager(m_pInstrumentPropPgMgr);
		
		if(SUCCEEDED(hr))
		{
			m_pInstrumentPropPgMgr->SetObject(this);
			pIPropSheet->SetActivePage( 0 ); 
		}

		pIPropSheet->Show(TRUE);
		pIPropSheet->Release();
	}

	return hr;
}

UINT AFX_CDECL StopResetFunction(LPVOID pParam)
{
	StopResetTimeEvent* resetTimeEvent = (StopResetTimeEvent*) pParam;
	
	WaitForSingleObject(resetTimeEvent->pInstrument->m_hStopEvent, resetTimeEvent->lWaitTime);

	resetTimeEvent->pInstrument->Stop( TRUE );
	TRACE("Killing update thread\n");
	::AfxEndThread(0);

	return 0;
}



// =================================================================================
// IDMUSProdTransport methods
// =================================================================================
HRESULT CInstrument::Play(BOOL /*fPlayFromStart*/)
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
	ResetEvent(m_hStopEvent);
	bool bOneShot = true;

	// Return failure if we don't have a valid port to play the instrument
	if(m_pComponent->m_lstDLSPortDownload.IsEmpty())
	{
		AfxMessageBox(IDS_ERR_NO_PORT, MB_OK | MB_ICONEXCLAMATION);
		return E_FAIL;
	}

    HRESULT hr = S_OK;
	if (m_pInstrumentCtrl->m_pInstrumentEditor->m_pCurRegion->m_pWave == NULL)
	{
		return hr;
	}
    
	int nRootNote = m_pInstrumentCtrl->m_pInstrumentEditor->m_pCurRegion->m_rWSMP.usUnityNote;
	
	// Check if the note we're playing belongs to the region or not
	// If not then change it to the nearest note in the region
	if(m_nMIDINote < m_pInstrumentCtrl->m_pInstrumentEditor->m_pCurRegion->m_rRgnHeader.RangeKey.usLow)
	{
		m_nMIDINote = m_pInstrumentCtrl->m_pInstrumentEditor->m_pCurRegion->m_rRgnHeader.RangeKey.usLow;
	}
	else if(m_nMIDINote > m_pInstrumentCtrl->m_pInstrumentEditor->m_pCurRegion->m_rRgnHeader.RangeKey.usHigh)
	{
		m_nMIDINote = m_pInstrumentCtrl->m_pInstrumentEditor->m_pCurRegion->m_rRgnHeader.RangeKey.usHigh;
	}

	// This is the number of semitones the note is shifted by
	// If the playing note is higher then the time to play the wave will go down by
	// 1.059463094359 for each semitone....
	long lPitchShift = m_nMIDINote - nRootNote ;
	int nShiftDirection = 1;
	if(lPitchShift != 0)
	{
		nShiftDirection = lPitchShift/abs(lPitchShift);
	}

	// 1.059463094359 is 12th root of 2; 0.9438743126817 is 1/12th root of 2
	// If the pitch is shifted up then we have to divide by 12th root of 2
	// If the pitch is shifted down then we have to divide by 12th root of 2
	float fTimeShiftFactor = (float)(nShiftDirection < 0 ? 1.059463094359 : 0.9438743126817);

	m_cMidiNoteOns[m_nMIDINote]++;
    hr = m_pComponent->PlayMIDIEvent(MIDI_NOTEON,
										(BYTE)m_nMIDINote,
										DEFAULT_VELOCITY, 
										5, // Give enough time for the UpdatePatch to apply
										IsDrum());   
    if (SUCCEEDED(hr))
    {
		if( (m_pInstrumentCtrl->m_pInstrumentEditor->m_pCurRegion->m_bOverrideWaveMoreSettings &&
		     m_pInstrumentCtrl->m_pInstrumentEditor->m_pCurRegion->m_rWSMP.cSampleLoops) ||	
			 (m_pInstrumentCtrl->m_pInstrumentEditor->m_pCurRegion->m_pWave->GetNumberOfLoops()) )
		{	// we're looping. Enable the Stop button.
			bOneShot = false;
		}
		if(bOneShot)
		{
			CWave* pPlayingWave = m_pInstrumentCtrl->m_pInstrumentEditor->m_pCurRegion->m_pWave;

			m_ResetCallBackParams.lWaitTime = (long) ((DWORD)1000*((float)pPlayingWave->m_dwWaveLength / pPlayingWave->m_rWaveformat.nSamplesPerSec));
			for(int nSemiTones = 0; nSemiTones < abs(lPitchShift); nSemiTones++)
				m_ResetCallBackParams.lWaitTime = (long) (m_ResetCallBackParams.lWaitTime * fTimeShiftFactor);

			
			m_ResetCallBackParams.pInstrument = this;

			CWinThread* pUpdateThread = ::AfxBeginThread(StopResetFunction, &m_ResetCallBackParams);
			pUpdateThread->m_bAutoDelete = TRUE;
		}

		m_pComponent->m_pIConductor->SetBtnStates(this, BS_PLAY_CHECKED | BS_STOP_ENABLED | BS_NO_AUTO_UPDATE);
    }
    return hr;
}



HRESULT CInstrument::Stop( BOOL /*fStopImmediate*/ )
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
	SetEvent(m_hStopEvent);
    HRESULT hr = S_OK;
    {
       hr = m_pComponent->PlayMIDIEvent(MIDI_NOTEOFF,
										(BYTE)m_nMIDINote,
										DEFAULT_VELOCITY,
             							6,
										IsDrum());
       if (SUCCEEDED(hr))
       {
            m_pComponent->m_pIConductor->SetBtnStates(this, BS_PLAY_ENABLED | BS_NO_AUTO_UPDATE);
			if (m_cMidiNoteOns[m_nMIDINote] > 0)
				m_cMidiNoteOns[m_nMIDINote]--;
       }
    }
    return S_OK;
}

HRESULT CInstrument::Transition ( )
{
    return E_NOTIMPL;
}

HRESULT CInstrument::Record( BOOL fPressed )
{
	return E_NOTIMPL;
}

HRESULT CInstrument::GetName(BSTR* pbstrName)
{
	return E_NOTIMPL;
}

HRESULT CInstrument::TrackCursor( BOOL fTrackCursor )
{
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument Additional functions

/////////////////////////////////////////////////////////////////////////////
// CInstrument::Save

HRESULT CInstrument::Save(IDMUSProdRIFFStream* pIRiffStream, bool fFullSave)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT			hr = S_OK;			// Return code
    MMCKINFO		ick;
    MMCKINFO		ck;
	DWORD			cb;
    CRegion *        pRegion;

	IStream*		pIStream;
	pIStream = pIRiffStream->GetStream();

    ick.fccType = FOURCC_INS ;
    hr = pIRiffStream->CreateChunk(&ick,MMIO_CREATELIST);
    if (FAILED(hr))
    {
		pIStream->Release();
        return (hr);
    }

    ck.ckid = FOURCC_INSH ;
    hr = pIRiffStream->CreateChunk(&ck, 0);
    if (FAILED(hr))
    {
		pIStream->Release();
        return (hr);
    }
	m_rInstHeader.cRegions = m_Regions.GetChannelCount();
	hr = pIStream->Write((LPSTR) &m_rInstHeader,sizeof(INSTHEADER), &cb);	
	if (FAILED(hr) || cb != sizeof(INSTHEADER))
	{
		pIStream->Release();
		return (hr);
	}
	if (FAILED(pIRiffStream->Ascend(&ck, 0)))
	{
		pIStream->Release();
		return (E_FAIL);
	}
	ck.fccType = FOURCC_LRGN ;
	hr = pIRiffStream->CreateChunk(&ck,MMIO_CREATELIST);
	if (FAILED(hr))
	{
		pIStream->Release();
		return (hr);
	}
	pRegion = m_Regions.GetHead();
	USHORT usRegionSaved = 1;
	for ( ; pRegion; pRegion = pRegion->GetNext())
	{
		hr = pRegion->Save(pIRiffStream, fFullSave, usRegionSaved++);
		if (FAILED(hr)) 
		{
			pIStream->Release();
			return(hr);
		}
	}
	pIRiffStream->Ascend(&ck, 0);

	POSITION position = m_lstArticulationLists.GetHeadPosition();
	while(position)
	{
		CArticulationList* pArticList = (CArticulationList*) m_lstArticulationLists.GetNext(position);
		ASSERT(pArticList);
		if(pArticList)
		{
			hr = pArticList->Save(pIRiffStream);
			if (FAILED(hr)) 
			{
				pIStream->Release();
				return(hr);
			}
		}
	}

    m_Info.Save(pIRiffStream);
    
    //save unknown chunks
    m_UnknownChunk.Save(pIRiffStream);

    pIRiffStream->Ascend(&ick, 0);
	pIStream->Release();
    return (hr);
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument::Load

HRESULT CInstrument::Load( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    IStream* pIStream = NULL;
	MMCKINFO ck;
	DWORD cb = 0;
	DWORD cSize = 0;
    HRESULT	hr = S_OK;

	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		return E_FAIL;
	}

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

    pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_OUTOFMEMORY;
	}

	// Remove all previous regions
	DeleteAllRegions();
	// Remove all previous articulation lists
	DeleteArticulationLists();

	ck.ckid = 0;
	ck.fccType = 0;
    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case FOURCC_INSH :
			{
			    if ( ck.cksize < sizeof(INSTHEADER) )
				{
					cSize = ck.cksize;
					if (ck.cksize < DLS_1_INST_HEADER_SIZE)
					{
						//To Do: message box
						//pIStream->Release();
						//return E_FAIL;
					}
				}
				else
				{
					cSize = sizeof(INSTHEADER);
				}
				hr = pIStream->Read( &m_rInstHeader, cSize, &cb );
                if( FAILED(hr) || cb != cSize )
				{
					pIStream->Release();
					return (hr);
				}
				break;
			}
			case FOURCC_LIST :
			{
				switch (ck.fccType)
				{
					case FOURCC_LRGN :
						if(FAILED(hr = m_Regions.Load(pIRiffStream,&ck)))
						{
							pIStream->Release();
							return hr;
						}
						break;
                    case mmioFOURCC('I','N','F','O') :
                        m_Info.Load(pIRiffStream,&ck);
                        break;

					case FOURCC_LART :
					case FOURCC_LAR2 :
                    {
						BOOL bDLS1 = FALSE;
						if(ck.fccType == FOURCC_LART)
						{
							bDLS1 = TRUE;
						}

						CArticulationList* pArticList = new CArticulationList(m_pComponent, this);
						if(pArticList)
						{
							pArticList->SetDLS1(bDLS1);
							
							hr = pArticList->Load(pIRiffStream, &ck);
							if(FAILED(hr))
							{
								delete pArticList;
								pIStream->Release();
								return hr;
							}
							m_lstArticulationLists.AddTail(pArticList);
						}
						else
						{
							pIStream->Release();
							return E_OUTOFMEMORY;
						}
			
                    }
					break;

                    default:
                        m_UnknownChunk.Load(pIRiffStream, &ck, TRUE);
                        break;
				}
				break;
			}

            case mmioFOURCC('e','d','i','t'): // Ignore edits while loading.
                break;

            default:
                m_UnknownChunk.Load(pIRiffStream, &ck, FALSE);
                break;

		}

        pIRiffStream->Ascend( &ck, 0 );
    }
	
	pIStream->Release();

	SetFirstGlobaArtAsCurrent();

	if (m_rInstHeader.Locale.ulBank & F_INSTRUMENT_DRUMS)
		m_Regions.SetDrumArticulation();

	m_Regions.ValidateUIA();

    return hr;
}

// Sets the first global articulation as the current articulation
// If there're no global articulations we look for the first region level articulation
void CInstrument::SetFirstGlobaArtAsCurrent()
{
	// Set the very first articulation as the current articulation for this instrument
	// We might not have any global articulations in which case we need to find the first one at the regions level
	if(m_lstArticulationLists.GetCount() != 0)
	{
		CArticulationList* pArticList = (CArticulationList*) m_lstArticulationLists.GetHead();
		ASSERT(pArticList);
		if(pArticList)
		{
			CArticulation* pArticulation = pArticList->GetHead();
			m_pCurArticulation = pArticulation;
			if(m_pCurArticulation && m_pInstrumentCtrl)
			{
				CInstrumentFVEditor* pEditor = m_pInstrumentCtrl->m_pInstrumentEditor;
				ASSERT(pEditor);
				if(pEditor)
				{
					pEditor->SetCurArticulation(pArticulation);
				}
			}
		}
	}
	else if(m_Regions.GetCount() > 0)
	{
		m_pCurArticulation = NULL;
		if(NoArtForEveryRegion())
		{
			CreateDefaultGlobalArticulation();
			ASSERT(m_pCurArticulation);
			if(m_pCurArticulation && m_pInstrumentCtrl)
			{
				m_pInstrumentCtrl->m_pInstrumentEditor->SetCurArticulation(m_pCurArticulation);
			}
		}
		else
		{
			CRegion* pRegion = m_Regions.GetHead();
			ASSERT(pRegion);
			pRegion->SetCurrentArtInEditor();
		}
	}
	else
	{
		CreateDefaultGlobalArticulation();
		ASSERT(m_pCurArticulation);
		if(m_pCurArticulation && m_pInstrumentCtrl)
		{
			m_pInstrumentCtrl->m_pInstrumentEditor->SetCurArticulation(m_pCurArticulation);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CInstrument::GetMaxVolume

GCENT CInstrument::GetMaxVolume()
{
	CRegion* pRegion;
	GCENT gcMax = -9600 << 16;
    
	pRegion = m_Regions.GetHead();
    for( ; pRegion != NULL; pRegion = pRegion->GetNext())
    {
		if(pRegion->m_rWSMP.lAttenuation > gcMax)
		{
			gcMax = pRegion->m_rWSMP.lAttenuation;
		}
	}

	return gcMax;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument::UpdatePatch

void CInstrument::UpdatePatch()
{
	DWORD       dwMIDIBank;
	WORD		nMIDIPatch;

	nMIDIPatch = (WORD) m_rInstHeader.Locale.ulInstrument;
    dwMIDIBank = m_rInstHeader.Locale.ulBank;

	m_pComponent->PlayMIDIEvent(MIDI_CCHANGE,CC_BANKSELECTH,(BYTE)((dwMIDIBank >> 8) & 0x7F),0, IsDrum());
    m_pComponent->PlayMIDIEvent(MIDI_CCHANGE,CC_BANKSELECTL,(BYTE)(dwMIDIBank & 0x7F),1, IsDrum());
    m_pComponent->PlayMIDIEvent(MIDI_PCHANGE,nMIDIPatch & 0x7F,0,2, IsDrum());
	m_pComponent->PlayMIDIEvent(MIDI_CCHANGE,CC_VOLUME,100,3, IsDrum());
	m_pComponent->PlayMIDIEvent(MIDI_CCHANGE,CC_EXPRESSION,127,4, IsDrum());
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument::UpdateInstrument

HRESULT CInstrument::UpdateInstrument()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	HRESULT hr = S_OK;

	if(m_pComponent->m_lstDLSPortDownload.IsEmpty())
		return E_FAIL;

	POSITION position = m_pComponent->m_lstDLSPortDownload.GetHeadPosition();
	while(position)
	{
		IDirectMusicPortDownload* pIDMPortDownload = m_pComponent->m_lstDLSPortDownload.GetNext(position);
		ASSERT(pIDMPortDownload);

        // Keep the old download ID
        if(FAILED(GetDMIDForPort(pIDMPortDownload, &m_dwId)))
        {
            return E_FAIL;
        }

		// Unload the old instrument
		if(FAILED(Unload(pIDMPortDownload)))
			return E_FAIL;

		// Compute the new buffer size
		hr = Size(m_dwDLSize);
		if (m_dwDLSize)
		{
			// Add prepend and append amount to download buffer size
			DWORD dwAppendValue = 0;
			if(FAILED(m_pComponent->GetAppendValueForPort(pIDMPortDownload, &dwAppendValue)))
				return E_FAIL;

			m_dwDLSize += dwAppendValue;

			// Allocate a download buffer
			IDirectMusicDownload* pIDMDownload;
			hr = pIDMPortDownload->AllocateBuffer(m_dwDLSize, &pIDMDownload);
			if (!SUCCEEDED(hr))
			{
				AfxMessageBox(IDS_ERR_UPDATE_ALLOC,MB_OK | MB_ICONEXCLAMATION);
				return hr;
			}
			
			// Add to the list of allocated buffers
			AllocatedPortBufferPair* pAllocatedBuffer = new AllocatedPortBufferPair(pIDMPortDownload, pIDMDownload);
			if(pAllocatedBuffer == NULL)
				return E_OUTOFMEMORY;

            pAllocatedBuffer->SetDMID(m_dwId);

			m_lstAllocatedBuffers.AddTail(pAllocatedBuffer);

			hr = Download(pIDMPortDownload);
			if (!SUCCEEDED(hr))
			{
				// Remove the added buffer
				if(pAllocatedBuffer)
					RemoveFromAllocatedBufferList(pAllocatedBuffer);

				AfxMessageBox(IDS_ERR_UPDATE_DOWNLOAD, MB_OK | MB_ICONEXCLAMATION);
			}
		}
		else
		{
	#ifdef _DEBUG
			AfxMessageBox(IDS_ERR_ZERO_SIZE_INS, MB_OK | MB_ICONEXCLAMATION);
	#endif
			hr = E_FAIL;
		}
	}
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument::GetName

void CInstrument::GetName()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CString		csFormatStr;
	CString		csDrumKitStr;
	char* pszTemp;
	DWORD	dwSize;

	csDrumKitStr.LoadString(IDS_DRUM_KIT_SUFFIX);
	m_Info.m_csName.TrimRight();
    if(m_Info.m_csName.IsEmpty() || m_Info.m_csName == "")
    {
		pszTemp = new char[BUFFER_128];
		csFormatStr.LoadString(IDS_BLANK_INST_NAME_FORMAT);
        sprintf(pszTemp,csFormatStr,
			    (m_rInstHeader.Locale.ulBank >> 8) & 0x7F,
				m_rInstHeader.Locale.ulBank & 0x7F,
				m_rInstHeader.Locale.ulInstrument);
    }
    else
    {
		csFormatStr.LoadString(IDS_INST_NAME_FORMAT);
		int nSeparatorIndex  = m_Info.m_csName.Find(_T(":"));
		if(nSeparatorIndex != -1)
			m_Info.m_csName = m_Info.m_csName.Left(nSeparatorIndex);
		dwSize = m_Info.m_csName.GetLength() + csFormatStr.GetLength() + csDrumKitStr.GetLength() + 3;
		pszTemp = new char[dwSize];
		
        sprintf(pszTemp,csFormatStr,(LPCTSTR)m_Info.m_csName,
                (m_rInstHeader.Locale.ulBank >> 8) & 0x7F,
                m_rInstHeader.Locale.ulBank & 0x7F,
                m_rInstHeader.Locale.ulInstrument);
    }
    
	if(F_INSTRUMENT_DRUMS & m_rInstHeader.Locale.ulBank)
    {
        strcat(pszTemp,": Drum Kit");
    }

	m_csName = pszTemp;
	delete pszTemp;
}

void CInstrument::GetName(CString& sName)
{
	sName = m_Info.m_csName;
}
/////////////////////////////////////////////////////////////////////////////
// CInstrument IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CInstrument IPersist::GetClassID

HRESULT CInstrument::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( pClsId != NULL );

    memset( pClsId, 0, sizeof( CLSID ) );

    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
//
// CInstrument IPersistStream::IsDirty
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrument::IsDirty()
{
    // I want to know if I am called
	ASSERT(FALSE);

	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CInstrument IPersistStream::GetSizeMax
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrument::GetSizeMax( ULARGE_INTEGER FAR* /*pcbSize*/ )
{
    // I want to know if I am called
	ASSERT(FALSE);

	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CInstrument IPersistStream::Load
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrument::Load(IStream* pIStream)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_POINTER;
	}

	ULONG ulBytesRead = 0;

	UINT nLayers = 4;
	HRESULT hr = pIStream->Read((void *)&nLayers, sizeof(UINT), &ulBytesRead);
	if(FAILED(hr) || ulBytesRead != sizeof(UINT)) 
	{
		return E_FAIL; 
	}

	m_Regions.SetNumberOfLayers(nLayers);
	
	// Check the number of articulation lists in the stream
	int nArtListCount = 0;
	hr = pIStream->Read((void *)&nArtListCount, sizeof(int), &ulBytesRead);
	if(FAILED(hr) || ulBytesRead != sizeof(int)) 
	{
		return E_FAIL; 
	}

	int nExistentArtLists = m_lstArticulationLists.GetCount();
	if(nArtListCount < nExistentArtLists)
	{
		for(int nCount = 0; nCount < (nExistentArtLists	- nArtListCount); nCount++)
		{
			CArticulationList* pArtList = (CArticulationList*)m_lstArticulationLists.RemoveHead();
			ASSERT(pArtList);
			CArticulation* pArt = (CArticulation*) pArtList->GetHead();
			while(pArt)
			{
				pArt->SetDeleteFlag(true);
				pArt = pArt->GetNext();
			}

			m_pComponent->m_pIFramework->RemoveNode(pArtList, FALSE);
			pArtList->Release();
		}
	}
	else if(nArtListCount > nExistentArtLists)
	{
		for(int nCount = 0; nCount < (nArtListCount - nExistentArtLists); nCount++)
		{
			InsertChildNode(NULL);
		}
	}

	// Now load up all Articulation lists
	POSITION position = m_lstArticulationLists.GetHeadPosition();
	while(position)
	{
		CArticulationList* pArtList = (CArticulationList*)m_lstArticulationLists.GetNext(position);
		ASSERT(pArtList);
		if(pArtList)
		{
			if(FAILED(hr = pArtList->Load(pIStream)))
			{
				return E_FAIL;
			}
		}
	}


	// Read the region selection values
	int nStartNote = 0;
	hr = pIStream->Read((void *)&nStartNote, sizeof(int), &ulBytesRead);
	if(FAILED(hr) || ulBytesRead != sizeof(int)) 
	{
		return E_FAIL; 
	}

	int nLayer = 0;
	hr = pIStream->Read((void *)&nLayer, sizeof(int), &ulBytesRead);
	if(FAILED(hr) || ulBytesRead != sizeof(int)) 
	{
		return E_FAIL; 
	}

	// Read the instrument header
	hr = pIStream->Read((void *)&m_rInstHeader, sizeof(INSTHEADER), &ulBytesRead);
	if(FAILED(hr) || ulBytesRead != sizeof(INSTHEADER))
	{
		return E_FAIL; 
	}

	// Manage loading the regions
	int nRegionCount = m_rInstHeader.cRegions;
	int nExistentRegions = m_Regions.GetCount();
	if(nRegionCount < nExistentRegions)
	{
		for(int nCount = 0; nCount < (nExistentRegions - nRegionCount); nCount++)
		{
			CRegion* pRegion = m_Regions.GetHead();
			if(pRegion == NULL)
			{
				break;
			}

			IDMUSProdNode* pParentNode = NULL;
			pRegion->GetParentNode(&pParentNode);
			if(pParentNode)
			{
				pRegion->m_bDeletingRegion = true;
				// Delete the region nodes
				m_Regions.m_pDLSComponent->m_pIFramework->RemoveNode(pRegion, false);

				CInstrumentFVEditor* pIE = GetInstrumentEditor();
				if(pIE)
				{
					pIE->m_RegionKeyBoard.DeleteRegion(short(pRegion->GetLayer()), pRegion->m_rRgnHeader.RangeKey.usLow);
					pIE->SendRegionChange();
				}
				// Remove from list
				m_Regions.Remove(pRegion);
				pRegion->Release();

				pParentNode->Release();
			}
		}
	}

	CRegion* pRgn = m_Regions.GetHead();
	for(; nRegionCount > 0 ; pRgn = pRgn->GetNext(), nRegionCount--)
	{
		if(pRgn == NULL)
		{
			pRgn = new CRegion(m_Regions.m_pDLSComponent);
			pRgn->m_pInstrument = this;
			m_Regions.InsertRegion((IDMUSProdNode*)pRgn, pRgn->GetLayer(), pRgn->m_rRgnHeader.RangeKey.usLow, pRgn->m_rRgnHeader.RangeKey.usHigh);
		}
		// Get pRgn IPersistStream interface pointer
		IPersistStream* pIPStream = NULL;
		if(SUCCEEDED(pRgn->QueryInterface(IID_IPersistStream, (void **) &pIPStream)))
		{
			hr = pIPStream->Load(pIStream);
			pIPStream->Release();
			if(FAILED(hr)) 
			{
				return E_FAIL;
			}
		}
		else
		{
			return E_FAIL; 
		}

		m_pComponent->m_pIFramework->RefreshNode(pRgn);
	}

	// This checks if a region was deleted last and 
	// if this comes through the undo then selects that region as the current region
	CRegion* pRegion = m_Regions.GetHead();
	for(; pRegion; pRegion = pRegion->GetNext())
	{
		if(pRegion->m_bLastDeleted)
		{
			nStartNote = pRegion->m_rRgnHeader.RangeKey.usLow;
			nLayer = pRegion->GetLayer();
			GetInstrumentEditor()->m_pCurRegion = NULL;
			pRegion->m_bLastDeleted = false;
			break;
		}
	}

	if(nStartNote >= 0 && nLayer >= 0 && GetInstrumentEditor() && !IsPropertyPageShowing())
		GetInstrumentEditor()->OnRegionSelectedChangedRegionRegionkeyboard(short(nLayer), short(nStartNote));

	if(!LoadString(pIStream, m_Info.m_csName))
		return false;

	if(!LoadString(pIStream, m_Info.m_csSubject))
		return false;

	if(!LoadString(pIStream, m_Info.m_csEngineer))
		return false;

	if(!LoadString(pIStream, m_Info.m_csCopyright))
		return false;

	if(!LoadString(pIStream, m_Info.m_csComment))
		return false;

	return S_OK;
}

bool CInstrument::LoadString(IStream* pIStream, CString& sData)
{
	ASSERT(pIStream);
	if(pIStream == NULL)
		return false;


	HRESULT hr = E_FAIL;
	DWORD	cb = 0;
	int		nStringLength = 0;
	CString sTempData; 

	hr = pIStream->Read((void*)&nStringLength, sizeof(int), &cb);
	if(FAILED(hr) || cb != sizeof(int))
		return false;

	char* szTempDataBuffer = sTempData.GetBufferSetLength(nStringLength);
	hr = pIStream->Read((void*)szTempDataBuffer, nStringLength, &cb);
	if(FAILED(hr) || cb != (DWORD)nStringLength)
		return false;

	sTempData.ReleaseBuffer(nStringLength);
	sData = sTempData;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
//
// CInstrument IPersistStream::Save
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrument::Save(IStream* pIStream, BOOL fClearDirty)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_POINTER;
	}

	ULONG ulBytesWritten = 0;

	// Save the number of layers
	UINT nLayers = m_Regions.GetNumberOfLayers();
	HRESULT hr = pIStream->Write((void*)&nLayers, sizeof(UINT), &ulBytesWritten);
	if(FAILED(hr) || ulBytesWritten != sizeof(UINT)) 
	{
		return E_FAIL;
	}
	

	// If there is no global articulation we have to note that on the  stream
	int nCount = GetArticulationCount() ? 1 : 0;
	int nArtListCount = m_lstArticulationLists.GetCount();
	hr = pIStream->Write((void*)&nArtListCount, sizeof(int), &ulBytesWritten);
	if(FAILED(hr) || ulBytesWritten != sizeof(int)) 
	{
		return E_FAIL;
	}

	POSITION position = m_lstArticulationLists.GetHeadPosition();
	while(position)
	{
		CArticulationList* pArtList = (CArticulationList*)m_lstArticulationLists.GetNext(position);
		if(pArtList)
		{
			if(FAILED(hr = pArtList->Save(pIStream, FALSE)))
			{
				return E_FAIL;
			}
		}
	}

	// Save the region that's currently selected so we can select it back on undo/redo
	int nStartNote = -1;
	int nLayer = -1;
	if(GetInstrumentEditor() && GetInstrumentEditor()->m_pCurRegion)
	{
		nStartNote = GetInstrumentEditor()->m_pCurRegion->m_rRgnHeader.RangeKey.usLow;
		nLayer = GetInstrumentEditor()->m_pCurRegion->GetLayer();
	}

	hr = pIStream->Write((void *)&nStartNote, sizeof(int), &ulBytesWritten);
	if(FAILED(hr) || ulBytesWritten != sizeof(int)) 
	{
		return E_FAIL;
	}

	hr = pIStream->Write((void *)&nLayer, sizeof(int), &ulBytesWritten);
	if(FAILED(hr) || ulBytesWritten != sizeof(int)) 
	{
		return E_FAIL;
	}


	m_rInstHeader.cRegions = m_Regions.GetCount();
	hr = pIStream->Write((void *)&m_rInstHeader, sizeof(INSTHEADER),&ulBytesWritten);
	if(FAILED(hr) || ulBytesWritten != sizeof(INSTHEADER)) 
	{
		return E_FAIL; 
	}

	for(CRegion* pRgn = m_Regions.GetHead(); pRgn; pRgn = pRgn->GetNext())
	{
		IPersistStream* pIPStream = NULL;
		if(SUCCEEDED(pRgn->QueryInterface(IID_IPersistStream, (void **) &pIPStream)))
		{
			hr = pIPStream->Save(pIStream, fClearDirty);
			pIPStream->Release();
			if (FAILED(hr)) 
			{
				return E_FAIL; 
			}
		}
		else
		{
			return (E_FAIL);  
		}
	}

	if(!SaveString(pIStream, m_Info.m_csName))
	{
		return E_FAIL;
	}

	if(!SaveString(pIStream, m_Info.m_csSubject))
	{
		return E_FAIL;
	}

	if(!SaveString(pIStream, m_Info.m_csEngineer))
	{
		return E_FAIL;
	}

	if(!SaveString(pIStream, m_Info.m_csCopyright))
	{
		return E_FAIL;
	}

	if(!SaveString(pIStream, m_Info.m_csComment))
	{
		return E_FAIL;
	}

	return S_OK;
}


bool CInstrument::SaveString(IStream* pIStream, CString sData)
{
	ASSERT(pIStream);
	if(pIStream == NULL)
		return false;

	DWORD	cb = 0;
	HRESULT hr = E_FAIL;
	int nStringLength = sData.GetLength() + 1;

	hr = pIStream->Write((void*) &nStringLength, sizeof(int), &cb);
	if(FAILED(hr) || cb != sizeof(int))
		return false;

	hr = pIStream->Write((void*)sData.GetBuffer(0), nStringLength, &cb);
	if(FAILED(hr) || cb != (DWORD)nStringLength)
		return false;
	
	return true;
}

void CInstrument::TurnOffMidiNotes()
{
	HRESULT hr;
	int i;

	for (i = 0; i < 128; i++)
	{
		if ( m_cMidiNoteOns[i] )
		{
			while ( m_cMidiNoteOns[i] )
			{
				hr = m_pComponent->PlayMIDIEvent(MIDI_NOTEOFF,
												BYTE(i),
												DEFAULT_VELOCITY,
             									0,
												IsDrum());
				m_cMidiNoteOns[i]--;
				if ( FAILED(hr) )
					break;
			}
		}
	}
}

HRESULT CInstrument::Write(void* pvoid, IDirectMusicPortDownload* pIDMPortDownLoad)
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

	DWORD dwCurIndex = 0;	// Used to determine what index to store offset in Offset Table
	DWORD dwCurOffset = 0;	// Offset relative to beginning of passed in memory

	HRESULT hr = S_OK;

	__try
	{
		// Write DMUS_DOWNLOADINFO		
		DMUS_DOWNLOADINFO* pDLInfo = (DMUS_DOWNLOADINFO *) pvoid;

		if(m_bNewFormat)
		{
			pDLInfo->dwDLType = DMUS_DOWNLOADINFO_INSTRUMENT2;
		}
		else
		{
			pDLInfo->dwDLType = DMUS_DOWNLOADINFO_INSTRUMENT;
		}

        if(FAILED(GetDMIDForPort(pIDMPortDownLoad, &m_dwId)))
        {
            return E_FAIL;
        }

		pDLInfo->dwDLId = m_dwId;
		pDLInfo->dwNumOffsetTableEntries = (m_cbSizeOffsetTable/sizeof(DWORD));
		pDLInfo->cbSize = m_dwDLSize;

		dwCurOffset += CHUNK_ALIGN(sizeof(DMUS_DOWNLOADINFO));

		DMUS_OFFSETTABLE* pDMOffsetTable = (DMUS_OFFSETTABLE *)(((BYTE*)pvoid) + dwCurOffset);

		// Increament pass the DMUS_OFFSETTABLE structure; we will fill the other members in later
		dwCurOffset += m_cbSizeOffsetTable;

		// First entry in ulOffsetTable is the first data chunk
		pDMOffsetTable->ulOffsetTable[0] = dwCurOffset;
		
		dwCurIndex = 1;
		
		// Write Instrument MIDI address
		DMUS_INSTRUMENT* pDMInstrument = (DMUS_INSTRUMENT*)(((BYTE *)pvoid) + dwCurOffset);
        pDMInstrument->ulFlags = 0;

		// Put data into format that is expected by synth
		pDMInstrument->ulPatch = ((m_rInstHeader.Locale.ulBank & 0x00007F7F) << 8) | (m_rInstHeader.Locale.ulInstrument) | (m_rInstHeader.Locale.ulBank & 0x80000000);

		dwCurOffset += CHUNK_ALIGN(sizeof(DMUS_INSTRUMENT));

		// Write regions
		pDMInstrument->ulFirstRegionIdx = 0;
		CRegion* pRegion = m_Regions.GetHead();
			
		// Make sure the next chunk can also be downloaded.
        while (pRegion && (pRegion->Count() == 0))
            pRegion = pRegion->GetNext();

		LONG iChannel = 0;
		while (pRegion)
			{
			LONG cChannels = pRegion->GetChannelCount();
			ASSERT(cChannels >= 1); // channel count not supported

			CRegion *pNextRegion;
			DWORD dwIndexNextRegion;
			LONG iNextChannel;
			if (iChannel+1 == cChannels)
				{
				iNextChannel = 0;
				pNextRegion = pRegion->GetNext();

				// Make sure the next chunk can also be downloaded.
				while (pNextRegion && (pNextRegion->Count() == 0))
					pNextRegion = pNextRegion->GetNext();
				}
			else
				{
				iNextChannel = iChannel + 1;
				pNextRegion = pRegion;
				}

			dwIndexNextRegion = pNextRegion ? dwCurIndex + pRegion->Count() : 0;

			if(pDMInstrument->ulFirstRegionIdx == 0)
				pDMInstrument->ulFirstRegionIdx = dwCurIndex;

			pDMOffsetTable->ulOffsetTable[dwCurIndex] = dwCurOffset;
		
			hr = pRegion->Write(((BYTE *)pvoid + dwCurOffset),
								iChannel,
								&dwCurOffset,
								pDMOffsetTable->ulOffsetTable,
								&dwCurIndex,
								dwIndexNextRegion,
                                pIDMPortDownLoad);
			if(FAILED(hr))
				break;

			iChannel = iNextChannel;
			pRegion = pNextRegion;
			}

		pDMInstrument->ulFirstExtCkIdx = 0;

		if(SUCCEEDED(hr))
		{
			DWORD	dwSize = 0;
			// Write copyright information
			if(!m_Info.m_csCopyright.IsEmpty())
			{
				dwSize = m_Info.m_csCopyright.GetLength() + 1;
				pDMOffsetTable->ulOffsetTable[dwCurIndex] = dwCurOffset;
				pDMInstrument->ulCopyrightIdx = dwCurIndex;
				CopyMemory((BYTE*)pvoid + dwCurOffset,m_Info.m_csCopyright.GetBuffer(0),dwSize);
				dwCurIndex++;
				m_Info.m_csCopyright.ReleaseBuffer();
			}
			// If instrument does not have one use collection's
			else if(m_pCollection && !m_pCollection->m_Info.m_csCopyright.IsEmpty())
			{
				dwSize = m_pCollection->m_Info.m_csCopyright.GetLength() + 1;
				pDMOffsetTable->ulOffsetTable[dwCurIndex] = dwCurOffset;
				pDMInstrument->ulCopyrightIdx = dwCurIndex;
				CopyMemory((BYTE*)pvoid + dwCurOffset,m_pCollection->m_Info.m_csCopyright.GetBuffer(0),dwSize);
				dwCurIndex++;
				m_pCollection->m_Info.m_csCopyright.ReleaseBuffer();
			}
			else
			{
				pDMInstrument->ulCopyrightIdx = 0;
			}
			dwCurOffset += dwSize;
		}
		if(SUCCEEDED(hr))
		{
			pDMInstrument->ulGlobalArtIdx = 0;
			UINT nArtCount = GetArticulationCount();
			if(nArtCount > 0)
			{
				POSITION position = m_lstArticulationLists.GetHeadPosition();
				while(position)
				{
					CArticulationList* pArtList = (CArticulationList*) m_lstArticulationLists.GetNext(position);
					if(pArtList && pArtList->IsConditionOK() && pArtList->GetCount() > 0)
					{
						pDMInstrument->ulGlobalArtIdx = dwCurIndex;

						if(FAILED(hr = pArtList->Write((BYTE*) pvoid, &dwCurOffset, &dwCurOffset, pDMOffsetTable->ulOffsetTable, &dwCurIndex)))
						{
							break;
						}
					}
				}
			}
			else
			{
				pDMInstrument->ulGlobalArtIdx = 0;
			}
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		hr = E_UNEXPECTED;
	}
	if(FAILED(hr))
	{
		// If we fail we want to cleanup the contents of passed in buffer
		ZeroMemory(pvoid, m_dwDLSize);
	}
	return hr;
}

HRESULT CInstrument::Size(DWORD& dwSize)
{
	HRESULT hr = S_OK;
	DWORD dwOffsetCount = 0;
	DWORD dwCountExtChk = 0;
	DWORD dwCountRegion = 0;
	dwSize = 0;

	__try
	{
		//Calculate the space needed for DMUS_DOWNLOADINFO 
		dwSize += CHUNK_ALIGN(sizeof(DMUS_DOWNLOADINFO));

		// Calculate space needed for DMUS_INSTRUMENT
		dwSize += CHUNK_ALIGN(sizeof(DMUS_INSTRUMENT));
		dwOffsetCount++;

		// Calculate the space needed for Instrument's copyright
		if(!m_Info.m_csCopyright.IsEmpty())
		{
			dwSize += m_Info.m_csCopyright.GetLength() + 1; // add 1 for null
			dwOffsetCount++;
		}
		// If instrument does not have one use collection's
		else if(m_pCollection && !m_pCollection->m_Info.m_csCopyright.IsEmpty())
		{
			dwSize += m_pCollection->m_Info.m_csCopyright.GetLength() + 1; // add 1 for null
			dwOffsetCount++;
		}

		UINT nArtCount = GetArticulationCount();
		if(nArtCount > 0)
		{
			POSITION position = m_lstArticulationLists.GetHeadPosition();
			while(position)
			{
				CArticulationList* pArtList = (CArticulationList*) m_lstArticulationLists.GetNext(position);
				if(pArtList)
				{
					dwSize += pArtList->Size();
					dwOffsetCount += pArtList->Count();
				}
			}
		}

		// Calculate the space needed for Instrument's regions 
		CRegion* pRegion = m_Regions.GetHead();
		for(; pRegion; pRegion = pRegion->GetNext())
		{
			LONG cChannels = pRegion->GetChannelCount();
			dwSize += pRegion->Size()*cChannels;
			dwOffsetCount += pRegion->Count()*cChannels;
			dwCountRegion += cChannels;
		}

		// Calculate the space needed for offset table
		m_cbSizeOffsetTable = CHUNK_ALIGN(dwOffsetCount * sizeof(ULONG));
		dwSize += m_cbSizeOffsetTable;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		dwSize = 0;
		hr = E_UNEXPECTED;
	}
	return hr;
}

HRESULT CInstrument::DM_Init(IDirectMusicPortDownload* pIDMPortDownLoad)
{
	HRESULT	hr = E_FAIL;

	// Do nothing if we don't have a valid port to download to...
	if(pIDMPortDownLoad == NULL)
		return E_FAIL;

	m_bNewFormat = FALSE;
	IKsControl* pControl = NULL;
	// Query for IKsControl interface
    if(SUCCEEDED(pIDMPortDownLoad->QueryInterface(IID_IKsControl, (void**)&pControl))) 
    { 
		KSPROPERTY	ksp;
		DWORD dwFlags = 0;
		DWORD cb = 0;

		// Ask about GM 
		ksp.Set   = GUID_DMUS_PROP_INSTRUMENT2; 
		ksp.Id    = 0; 
		ksp.Flags = KSPROPERTY_TYPE_BASICSUPPORT;
		HRESULT hr = pControl->KsProperty(&ksp, sizeof(ksp), &dwFlags, sizeof(dwFlags), &cb);
		if(SUCCEEDED(hr) || (cb >= sizeof(dwFlags)))
		{
			// Set is supported
			m_bNewFormat = (BOOL)(dwFlags & KSPROPERTY_TYPE_GET);
		}
		pControl->Release();
    }
    
	hr = Size(m_dwDLSize);
	if (m_dwDLSize)
	{
		// Add prepend and append amount to download buffer size
		DWORD dwAppendValue = 0;
		if(FAILED(m_pComponent->GetAppendValueForPort(pIDMPortDownLoad, &dwAppendValue)))
			return E_FAIL;
		
		m_dwDLSize += dwAppendValue;

		// Allocate a download buffer
		IDirectMusicDownload* pIDMDownload;
		hr = pIDMPortDownLoad->AllocateBuffer(m_dwDLSize, &pIDMDownload);
		if (!SUCCEEDED(hr))
		{
			AfxMessageBox(IDS_ERR_INSUFFICIENT_MEMORY, MB_OK | MB_ICONEXCLAMATION);
			return E_OUTOFMEMORY;
		}

		// Add to the list of allocated buffers
		AllocatedPortBufferPair* pAllocatedBuffer = GetAllocatedBufferForPort(pIDMPortDownLoad);
		if(pAllocatedBuffer == NULL)
		{
			pAllocatedBuffer = new AllocatedPortBufferPair(pIDMPortDownLoad, pIDMDownload);
			if(pAllocatedBuffer == NULL)
				return E_OUTOFMEMORY;

			m_lstAllocatedBuffers.AddTail(pAllocatedBuffer);
		}

		hr = pIDMPortDownLoad->GetDLId(&m_dwId,1);
		if (!SUCCEEDED(hr))
		{
			// Remove the added buffer
			if(pAllocatedBuffer)
				RemoveFromAllocatedBufferList(pAllocatedBuffer);

#ifdef _DEBUG
			AfxMessageBox(IDS_ERR_DM_ID, MB_OK | MB_ICONEXCLAMATION);
#endif
			return hr;
		}

        pAllocatedBuffer->SetDMID(m_dwId);
	}
	else //if (m_dwDLSize)
	{
#ifdef _DEBUG
		AfxMessageBox(IDS_ERR_ZERO_SIZE_INS, MB_OK | MB_ICONEXCLAMATION);
#endif
		hr = E_FAIL;
	}
	return hr;
}

HRESULT CInstrument::Download(IDirectMusicPortDownload* pIDMPortDownLoad)
{
	void*		pBuffer=NULL;
	DWORD		dwSize;
	HRESULT		hr = S_OK;

	// Do nothing if we don't have a valid port to download to...
	if(pIDMPortDownLoad == NULL)
		return E_FAIL;
	
	// Get the allocated buffer
	AllocatedPortBufferPair* pAllocatedBuffer = GetAllocatedBufferForPort(pIDMPortDownLoad);
	ASSERT(pAllocatedBuffer);
	if(pAllocatedBuffer == NULL)
		return E_FAIL;

	IDirectMusicDownload* pIDMDownload = pAllocatedBuffer->m_pAllocatedBuffer;
	
	// Fail if we haven't allocated a buffer for download
	if(pIDMDownload == NULL)
		return E_FAIL;

	hr = pIDMDownload->GetBuffer(&pBuffer,&dwSize);
	if ( !SUCCEEDED(hr) ||(pBuffer == NULL) || (dwSize < m_dwDLSize) )
	{
#ifdef _DEBUG
		AfxMessageBox(IDS_ERR_GET_DOWNLOAD_BUFFER, MB_OK | MB_ICONEXCLAMATION);
#endif
		return E_FAIL;
	}
	// Make sure we start writing pass the prepend portion of the download buffer
	if ( !SUCCEEDED(hr = Write(((BYTE *)pBuffer), pIDMPortDownLoad)))
	{
#ifdef _DEBUG
		AfxMessageBox(IDS_ERR_DOWNLOAD_WRITE, MB_OK | MB_ICONEXCLAMATION);
#endif
		return hr;
	}

	if ( !SUCCEEDED( hr = pIDMPortDownLoad->Download(pIDMDownload) ) )
	{
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
#ifdef _DEBUG
		AfxMessageBox(iErrorStringResource, MB_OK | MB_ICONEXCLAMATION);
#endif
	}
	

	return hr;
}

bool CInstrument::ValidatePatch()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		return true;
	}

	if(m_pComponent->IsNoShowCollectionPatchConflict())
	{
		return true;
	}

	CInstrument* pInstrumentCollision = NULL;
	BYTE		bMSB, bLSB, bPatch;
	DWORD		dwDrum;
	bool		bIgnore = false;

	ASSERT(m_pCollection);

	HRESULT hr = m_pComponent->IsValidPatch(this, m_rInstHeader.Locale.ulBank, m_rInstHeader.Locale.ulInstrument, m_pCollection, &pInstrumentCollision);
	if ( SUCCEEDED(hr) )
	{
		return true;
	}
	if ( m_pCollection->m_fPatchResolution == DLS_DLG_FIX_ALL)
	{//auto fix
		bMSB = (BYTE) ((m_rInstHeader.Locale.ulBank & 0x00007F00) >> 8);
		bLSB = (BYTE) (m_rInstHeader.Locale.ulBank & 0x0000007F);
		bPatch = (BYTE) (m_rInstHeader.Locale.ulInstrument & 0x0000007F);
		dwDrum =  m_rInstHeader.Locale.ulBank & 0x80000000;
		hr = m_pComponent->GetNextValidPatch(dwDrum, &bMSB, &bLSB, &bPatch);
		if (SUCCEEDED(hr))
		{
			m_rInstHeader.Locale.ulBank = MAKE_BANK(dwDrum, bMSB, bLSB);
			m_rInstHeader.Locale.ulInstrument = bPatch;
			return false;
		}
		return true;
	}
	if ( (hr & SC_PATCH_DUP_GM) && (m_pCollection->m_fPatchResolution & DLS_DLG_IGNORE_ALL_GM) )
	{
		hr &= ~SC_PATCH_DUP_GM;
	}
	if ( (hr & SC_PATCH_DUP_COLLECTION) && (m_pCollection->m_fPatchResolution & DLS_DLG_IGNORE_ALL_COLLECTION) )
	{
		hr &= ~SC_PATCH_DUP_COLLECTION;
	}
	if ( (hr & SC_PATCH_DUP_SYNTH) && (m_pCollection->m_fPatchResolution & DLS_DLG_IGNORE_ALL_SYNTH) )
	{
		hr &= ~SC_PATCH_DUP_SYNTH;
	}
	if (DLS_ERR_FIRST == hr)
	{
		return true;
	}
	CPatchConflictDlg* pConflictDlg = new CPatchConflictDlg(hr, pInstrumentCollision, m_pCollection, this);
	int retval = pConflictDlg->DoModal();
	delete pConflictDlg;

	switch (retval)
	{
		case IDOK:
			m_pCollection->SetDirtyFlag();
			return false;
			break;
		case ID_BTN_IGNORE:
			return true;
			break;
		case DLS_DLG_FIX_ALL:
			m_pCollection->m_fPatchResolution = DLS_DLG_FIX_ALL;
			m_pCollection->SetDirtyFlag();
			return false;
			break;
		default:
			m_pCollection->m_fPatchResolution |= retval;
			return true;
			break;
	}
}

HRESULT CInstrument::OnViewProperties()
{

	ASSERT(m_pComponent->m_pIFramework != NULL );

	HRESULT hr = E_FAIL;

	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( m_pComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		if( pIPropSheet->IsShowing() == S_OK )
		{
			IDMUSProdPropPageObject* pIPageObject;

			if( SUCCEEDED ( this->QueryInterface( IID_IDMUSProdPropPageObject, (void **)&pIPageObject ) ) )
			{
				if( SUCCEEDED ( pIPageObject->OnShowProperties() ) )
				{
					hr = S_OK;
				}

				pIPageObject->Release();
			}
		}
		else
		{
			hr = S_OK;
		}

		pIPropSheet->Release();
	}

	return hr;
}


BOOL CInstrument::VerifyDLS()
{
	UINT uMessageBoxOptions = MB_OKCANCEL | MB_ICONEXCLAMATION;
	BOOL bRet = TRUE;
	CString sCollectionName = m_pCollection->m_Info.m_csName + ":";
	CString sContinuePrompt;
	sContinuePrompt.LoadString(IDS_CONTINUE_VERIFYDLS);

	if(FAILED(CheckForOverlappedRegions()))
	{
		bRet = FALSE;
		CString sErrorMessage;
		sErrorMessage.Format(IDS_ERR_INST_OVERLAPPING_REGIONS, m_Info.m_csName);
		sErrorMessage = sCollectionName + sErrorMessage + "\n" + sContinuePrompt;
		if (IDCANCEL == AfxMessageBox(sErrorMessage, uMessageBoxOptions, 0))
			return bRet;
	}

    if(F_INSTRUMENT_DRUMS & m_rInstHeader.Locale.ulBank)
    {
        if (m_Regions.GetCount() > 128)
        {
			bRet = FALSE;
			CString sErrorMessage;
			sErrorMessage.Format(IDS_ERR_DRUMS_MORE_REGIONS, m_Info.m_csName, m_Regions.GetCount());
			sErrorMessage = sCollectionName + sErrorMessage + "\n" + sContinuePrompt;
			if(IDCANCEL == AfxMessageBox(sErrorMessage, uMessageBoxOptions, 0))
				return bRet;
        }
    }
    else
    {
        DWORD dwLocalA = 0;
        CRegion* pRegion = m_Regions.GetHead();
        for(;pRegion;pRegion = pRegion->GetNext())
        {
            if(pRegion->m_pArticulation != NULL)
                dwLocalA++;
        }

        if(dwLocalA > 0)
        {
			bRet = FALSE;
			CString sErrorMessage;
			sErrorMessage.Format(IDS_ERR_INVALID_NUMBER_ARTICULATIONS, m_Info.m_csName, dwLocalA);
			sErrorMessage = sCollectionName + sErrorMessage + "\n" + sContinuePrompt;
			if (IDCANCEL == AfxMessageBox(sErrorMessage, uMessageBoxOptions,0))
				return bRet;
        }

        if(m_Regions.IsEmpty() || GetDLS1RegionCount() == 0)
        {
			bRet = FALSE;
			CString sErrorMessage;
			sErrorMessage.Format(IDS_ERR_INST_NO_REGIONS, m_Info.m_csName);
			sErrorMessage = sCollectionName + sErrorMessage + "\n" + sContinuePrompt;
			if (IDCANCEL == AfxMessageBox(sErrorMessage, uMessageBoxOptions, 0))
				return bRet;
        }

        if(m_Regions.GetCount() > 16)
        {
			bRet = FALSE;
			CString sErrorMessage;
			sErrorMessage.Format(IDS_ERR_INST_INVALID_NUMBER_REGIONS, m_Info.m_csName, m_Regions.GetCount());
			sErrorMessage = sCollectionName + sErrorMessage + "\n" + sContinuePrompt;
			if(IDCANCEL == AfxMessageBox(sErrorMessage, uMessageBoxOptions, 0))
				return bRet;
        }
    }
    return bRet;
}

HRESULT CInstrument::CheckForOverlappedRegions()
{
	CRegion* pRegion = m_Regions.GetHead();
    for(;pRegion;pRegion = pRegion->GetNext())
    {
		CRegion* pCompare = pRegion->GetNext();
		for(;pCompare;pCompare = pCompare->GetNext())
		{
			if(((pCompare->m_rRgnHeader.RangeKey.usLow  <= pRegion->m_rRgnHeader.RangeKey.usHigh) &&
				(pCompare->m_rRgnHeader.RangeKey.usLow   >= pRegion->m_rRgnHeader.RangeKey.usLow)) ||
				((pCompare->m_rRgnHeader.RangeKey.usHigh <= pRegion->m_rRgnHeader.RangeKey.usHigh) &&
				(pCompare->m_rRgnHeader.RangeKey.usHigh  >= pRegion->m_rRgnHeader.RangeKey.usLow)) ||
				((pRegion->m_rRgnHeader.RangeKey.usLow	 <= pCompare->m_rRgnHeader.RangeKey.usHigh) &&
				(pRegion->m_rRgnHeader.RangeKey.usLow	 >= pCompare->m_rRgnHeader.RangeKey.usLow)) ||
				((pRegion->m_rRgnHeader.RangeKey.usHigh  <= pCompare->m_rRgnHeader.RangeKey.usHigh) &&
				(pRegion->m_rRgnHeader.RangeKey.usHigh	 >= pCompare->m_rRgnHeader.RangeKey.usLow)))
			{
				return E_FAIL;
			} 
		}
    }
	return S_OK;
}


DWORD CInstrument::GetDLS1RegionCount()
{
	DWORD dwRgnCnt = 0;
	CRegion* pRegion = m_Regions.GetHead();
	while(pRegion)
	{
		if(pRegion->GetLayer() == 0)
		{
			dwRgnCnt++;
		}

		pRegion = pRegion->GetNext();
	}

	return dwRgnCnt;
}

// Gets the region that contains the passed note
CRegion* CInstrument::GetRegion(int nNote)
{
	CRegion* pRegion = m_Regions.GetHead();
    for(;pRegion;pRegion = pRegion->GetNext())
    {
		if(pRegion->m_rRgnHeader.RangeKey.usLow <= nNote && pRegion->m_rRgnHeader.RangeKey.usHigh >= nNote)
			return pRegion;
	}

	return NULL;
}

// Updates the property page and refreshes the node
// Will not update the wavelink data if bWaveDataChanged is FALSE
void CInstrument::RefreshUI(bool bWaveDataChanged)
{
	// Refresh the Instrument's property page if it's showing
	if(IsPropertyPageShowing() == true)
	{
		m_pInstrumentPropPgMgr->RefreshData();
	}
		

	if(m_pComponent && m_pComponent->m_pIFramework)
	{
		m_pComponent->m_pIFramework->RefreshNode(this);
	}
		
	if(m_pInstrumentCtrl && m_pInstrumentCtrl->m_pInstrumentEditor)
	{
		// Keep the region that is currently selected so we can set it as selected back
		short nStartNote = m_pInstrumentCtrl->m_pInstrumentEditor->m_pCurRegion->m_rRgnHeader.RangeKey.usLow;
		short nLayer = short(m_pInstrumentCtrl->m_pInstrumentEditor->m_pCurRegion->GetLayer());

		m_pInstrumentCtrl->m_pInstrumentEditor->m_RegionKeyBoard.DeleteAllRegions();
		m_pInstrumentCtrl->m_pInstrumentEditor->SetRKBMap();

		m_pInstrumentCtrl->m_pInstrumentEditor->m_RegionKeyBoard.SetCurrentRegion(nLayer, nStartNote);
		
		if(bWaveDataChanged)
		{
			m_pInstrumentCtrl->m_pInstrumentEditor->RegionWaveChanged();
		}

		m_pInstrumentCtrl->m_pInstrumentEditor->RefreshRegion();
		m_pInstrumentCtrl->m_pInstrumentEditor->SetupArticulation();
	}

	// Resort the region nodes
	if(m_pComponent && m_pComponent->m_pIFramework)
	{
		m_pComponent->m_pIFramework->SortChildNodes(&m_Regions);
	}
}


void CInstrument::RefreshRegion()
{
	if(m_pComponent && m_pComponent->m_pIFramework)
		m_pComponent->m_pIFramework->RefreshNode(this);

	if(GetInstrumentEditor())
		GetInstrumentEditor()->RefreshRegion();
}

CInstrumentFVEditor* CInstrument::GetInstrumentEditor()
{
	if(m_pInstrumentCtrl)
		return m_pInstrumentCtrl->m_pInstrumentEditor;

	return NULL;
}

HRESULT CInstrument::SaveStateForUndo(UINT uUndoTextID)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pUndoMgr);
	if(m_pUndoMgr == NULL)
	{
		return E_FAIL;
	}
	
	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		return E_POINTER;
	}

	IDMUSProdFramework* pIFramework = m_pComponent->m_pIFramework;
	ASSERT(pIFramework);
	if(pIFramework == NULL)
	{
		return E_FAIL;
	}


	HRESULT hr = S_OK;
	CString	csMenuText;
 	csMenuText.LoadString(uUndoTextID);
	
	if(FAILED(hr = m_pUndoMgr->SaveState(this, csMenuText.GetBuffer(MENU_TEXT_SIZE))))
	{
		AfxMessageBox(IDS_ERR_INSUFFICIENT_MEMORY, MB_OK | MB_ICONEXCLAMATION);
		hr = E_FAIL;
	}

	
	return hr;
}

void CInstrument::PopUndoState()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pUndoMgr);
	if(m_pUndoMgr == NULL)
		return;

	m_pUndoMgr->DeleteState();
}

bool CInstrument::IsPropertyPageShowing()
{
	bool bPageIsUp = false;

	ASSERT(m_pComponent);
	IDMUSProdFramework* pIFramework = m_pComponent->m_pIFramework;
	ASSERT(pIFramework);

	if(pIFramework)
	{
		IDMUSProdPropSheet* pIPropSheet;
		if( SUCCEEDED ( pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			if( pIPropSheet->IsShowing() == S_OK && pIPropSheet->IsEqualPageManagerObject(this) == S_OK)
			{
				bPageIsUp = true;
			}
			
			pIPropSheet->Release();
		}
	}

	return bPageIsUp;
}

HRESULT CInstrument::Unload(IDirectMusicPortDownload* pIDMPortDownLoad)
{
	HRESULT hr = E_FAIL;

	ASSERT(pIDMPortDownLoad);
	if(pIDMPortDownLoad == NULL)
	{
		return E_POINTER;
	}

	AllocatedPortBufferPair* pAllocatedBuffer = GetAllocatedBufferForPort(pIDMPortDownLoad);
	if(pAllocatedBuffer == NULL)
	{
		return E_FAIL;
	}

	if(pAllocatedBuffer->m_pAllocatedBuffer)
	{
		hr = pAllocatedBuffer->m_pDownloadPort->Unload(pAllocatedBuffer->m_pAllocatedBuffer);
	}

	RemoveFromAllocatedBufferList(pAllocatedBuffer);

	return hr;
}

AllocatedPortBufferPair* CInstrument::GetAllocatedBufferForPort(IDirectMusicPortDownload* pIDMPortDownLoad)
{
	ASSERT(pIDMPortDownLoad);
	if(pIDMPortDownLoad == NULL)
	{
		return NULL;
	}

	POSITION position = m_lstAllocatedBuffers.GetHeadPosition();
	while(position)
	{
		AllocatedPortBufferPair* pAllocatedBuffer = (AllocatedPortBufferPair*)m_lstAllocatedBuffers.GetNext(position);
		
		ASSERT(pAllocatedBuffer);
		if(pAllocatedBuffer->m_pDownloadPort == pIDMPortDownLoad)
		{
			return pAllocatedBuffer;
		}
	}

	return NULL;
}

void CInstrument::RemoveFromAllocatedBufferList(AllocatedPortBufferPair* pAllocatedBuffer)
{
	ASSERT(pAllocatedBuffer);
	if(pAllocatedBuffer == NULL)
	{
		return;
	}

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

CInstrumentRegions* CInstrument::GetRegions()
{
	return &m_Regions;
}

BOOL CInstrument::SupportsNewFormat()
{
	return m_bNewFormat;
}

UINT CInstrument::GetArticulationCount()
{
	UINT nGlobalArticulations = 0;

	POSITION position = m_lstArticulationLists.GetHeadPosition();
	while(position)
	{
		CArticulationList* pArticList = (CArticulationList*) m_lstArticulationLists.GetNext(position);
		ASSERT(pArticList);
		if(pArticList)
		{
			nGlobalArticulations += pArticList->GetCount();
		}
	}

	return nGlobalArticulations;
}

CArticulation* CInstrument::GetCurrentArticulation()
{
	return m_pCurArticulation;
}

void CInstrument::CreateDefaultGlobalArticulation()
{
	CArticulationList* pArticList = new CArticulationList(m_pComponent, this);
	m_lstArticulationLists.AddTail(pArticList);
	m_pCurArticulation = (CArticulation*) pArticList->GetHead();
}

void CInstrument::DeleteAllRegions()
{
	while(!m_Regions.IsEmpty())
	{
		CRegion* pRegion = m_Regions.GetHead();
		if(FAILED(pRegion->DeleteNode(FALSE)))
		{
			m_Regions.Remove(pRegion);
			delete pRegion;
		}
	}
}

void CInstrument::DeleteArticulationLists()
{
	// Instrument's DeleteChildNode method will remove 
	// this from the list of articulation lists also 
	while(!m_lstArticulationLists.IsEmpty())
	{
		CArticulationList* pArticList = (CArticulationList*) m_lstArticulationLists.GetHead();
		if(FAILED(pArticList->DeleteNode(FALSE)))
		{
			POSITION position = m_lstArticulationLists.Find(pArticList);
			if(position)
			{
				m_lstArticulationLists.RemoveAt(position);
			}
			delete pArticList;
		}
	}

	// The current articulation is already pfft! 
	m_pCurArticulation = NULL;
}

void CInstrument::SetCurrentArticulation(CArticulation* pArticulation)
{
	m_pCurArticulation = pArticulation;
	if(m_pCurArticulation && m_pInstrumentCtrl)
	{
		m_pInstrumentCtrl->m_pInstrumentEditor->SetCurArticulation(pArticulation);
	}

	UpdateInstrument();
}

HRESULT CInstrument::GetListOfConditions(CPtrList* pConditionList)
{
	ASSERT(pConditionList);
	if(pConditionList == NULL)
	{
		return E_POINTER;
	}
	
	// Get the conditions from all regions 
	CRegion* pRegion = m_Regions.GetHead();
	while(pRegion)
	{
		if(FAILED(pRegion->GetListOfConditions(pConditionList)))
		{
			return E_FAIL;
		}

		pRegion = pRegion->GetNext();
	}

	// ...and all articulation lists...
	POSITION position = m_lstArticulationLists.GetHeadPosition();
	while(position)
	{
		CArticulationList* pArticList = (CArticulationList*)m_lstArticulationLists.GetNext(position);
		ASSERT(pArticList);
		if(pArticList)
		{
			CConditionalChunk* pCondition = pArticList->GetConditionalChunk();
			if(pCondition)
			{
				pConditionList->AddTail(pCondition);
			}
		}
	}

	return S_OK;
}

CCollection* CInstrument::GetCollection()
{
	ASSERT(m_pCollection);
	return m_pCollection;
}

void CInstrument::OnConditionConfigChanged(CSystemConfiguration* pCurrentConfig, bool bRefreshNode)
{
	ASSERT(pCurrentConfig);
	if(pCurrentConfig == NULL)
	{
		return;
	}

	CRegion* pRegion = m_Regions.GetHead();
	while(pRegion)
	{
		pRegion->OnConditionConfigChanged(pCurrentConfig, bRefreshNode);
		pRegion = pRegion->GetNext();
	}

	
	POSITION position = m_lstArticulationLists.GetHeadPosition();
	while(position)
	{
		CArticulationList* pArtList = (CArticulationList*)m_lstArticulationLists.GetNext(position);
		ASSERT(pArtList);
		if(pArtList)
		{
			pArtList->OnConditionConfigChanged(pCurrentConfig, bRefreshNode);
		}
	}
	
	if(m_pIParentNode)
	{
		UpdateInstrument();
	}

}


bool CInstrument::GetAuditionMode()
{
	return m_bAuditionMode;
}

void CInstrument::SetAuditionMode(bool bAuditionMode)
{
	m_bAuditionMode = bAuditionMode;
	UpdateInstrument();
}

void CInstrument::CheckRegionsForLocalArts()
{
	if(GetArticulationCount() >= 1)
	{
		return;
	}

	CRegion* pRegion = m_Regions.GetHead();
	while(pRegion)
	{
		UINT nArtCount = pRegion->GetArticulationCount();
		ASSERT(nArtCount);
		pRegion->ValidateUIA();
		pRegion = pRegion->GetNext();
	}
}

BOOL CInstrument::NoArtForEveryRegion()
{
	CRegion* pRegion = m_Regions.GetHead();
	while(pRegion)
	{
		if(pRegion->GetArticulationCount() == 0)
		{
			return TRUE;
		}
		pRegion = pRegion->GetNext();
	}

	return FALSE;
}

HRESULT CInstrument::GetDMIDForPort(IDirectMusicPortDownload* pIDMDownloadPort, DWORD* pdwDMID)
{
    ASSERT(pIDMDownloadPort);
    if(pIDMDownloadPort == NULL)
    {
        return E_POINTER;
    }

    ASSERT(pdwDMID);
    if(pdwDMID == NULL)
    {
        return E_POINTER;
    }

    POSITION position = m_lstAllocatedBuffers.GetHeadPosition();
    while(position)
    {
        AllocatedPortBufferPair* pBuffer = m_lstAllocatedBuffers.GetNext(position);
        ASSERT(pBuffer);
        if(pBuffer->m_pDownloadPort == pIDMDownloadPort)
        {
            *pdwDMID = pBuffer->GetDMID();
            return S_OK;
        }
    }

    return E_FAIL;
}
