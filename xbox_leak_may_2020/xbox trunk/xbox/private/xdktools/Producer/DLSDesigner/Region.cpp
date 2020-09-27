// Region.cpp : implementation file
//

#include "stdafx.h"
#include "DLSDesignerDLL.h"
#include "DLSDesigner.h"
#include "Collection.h"
#include "Instrument.h"
#include "Region.h"
#include "ArticulationList.h"
#include "Articulation.h"
#include "WaveNode.h"
#include "Wave.h"
#include "MonoWave.h"
#include "StereoWave.h"
#include "ConditionalChunk.h"

#include "InstrumentFVEditor.h"
#include "InstrumentCtl.h"

#include "DLSLoadSaveUtils.h"
#include "DlsDefsPlus.h"

#include "RegionPropPgMgr.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CRegion constructor/destructor

CRegion::CRegion(CDLSComponent* pComponent, bool bDLS1Region) :
m_pInstrument(NULL),
m_pWave(NULL),
m_usRegionSaved(0),
m_pArticulation(NULL),
m_pIRootNode(NULL),
m_pIParentNode(NULL),
m_pRegionPropPgMgr(NULL),
m_pInstrumentFVEditor(NULL),
m_bOverrideWaveAttenuation(false),
m_bOverrideWaveFineTune(false),
m_bOverrideWaveRootNote(false),
m_bOverrideWaveMoreSettings(false),
m_bDeletingRegion(false),
m_bUseInstrumentArticulation(true),
m_bLastDeleted(FALSE),
m_bDLS1Region(bDLS1Region),
m_bConditionOK(TRUE),
m_nLayer(0),
m_pConditionalChunk(NULL)
{
	ASSERT( pComponent != NULL );

    m_dwRef = 0;
	AddRef();

	m_pComponent = pComponent;
	m_pComponent->AddRef();

	// Create a conditional chunk
	m_pConditionalChunk = new CConditionalChunk(m_pComponent);

    m_rRgnHeader.usKeyGroup = 0;
    m_rRgnHeader.fusOptions = F_RGN_OPTION_SELFNONEXCLUSIVE;
    m_rRgnHeader.RangeKey.usLow = 0;
    m_rRgnHeader.RangeKey.usHigh = 127;
    m_rRgnHeader.RangeVelocity.usLow = 0;
    m_rRgnHeader.RangeVelocity.usHigh = 127;
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
    m_rWaveLink.ulChannel = WAVELINK_CHANNEL_LEFT;
    m_rWaveLink.fusOptions = 0;
    m_rWaveLink.usPhaseGroup = 0;
    m_rWaveLink.ulTableIndex = 0;

	if(m_pRegionPropPgMgr == NULL)
	{
		m_pRegionPropPgMgr = new CRegionPropPgMgr();
	}
	else
	{
		m_pRegionPropPgMgr->AddRef();
	}
}

CRegion::~CRegion()
{
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
			if(m_pRegionPropPgMgr)
			{
				m_pRegionPropPgMgr->RemoveObject(this);
			}
		}
		pIPropSheet->Release();
	}
	
	if(m_pRegionPropPgMgr)
	{
		DWORD refCount = m_pRegionPropPgMgr->Release();
		if(refCount == 0)
		{
			m_pRegionPropPgMgr = NULL;
		}
	}	
	
	while(!m_lstArticulationLists.IsEmpty())
	{
		CArticulationList* pArticList = (CArticulationList*) m_lstArticulationLists.RemoveHead();
		pArticList->Release();
	}

	if(m_pComponent)
	{
		m_pComponent->Release();
	}

	if(m_pWave)
	{
		CWaveNode* pWaveNode = m_pWave->GetNode();
		if(pWaveNode)
		{
			pWaveNode->Release();
		}
	}

	if(m_pConditionalChunk)
	{
		delete m_pConditionalChunk;
		m_pConditionalChunk = NULL;
	}
}

void CRegion::SetInitialWave(CWave* pWave)
{
	ASSERT(pWave);
	if(pWave == NULL)
		return;

	m_pWave = pWave;

	WSMPL waveWSMP = m_pWave->GetWSMPL();
	WLOOP waveLoop = m_pWave->GetWLOOP();

	m_rWSMP.cSampleLoops = waveWSMP.cSampleLoops;
	m_rWLOOP.ulStart = waveLoop.ulStart;
	m_rWLOOP.ulLength = waveLoop.ulLength;
	m_rWSMP.fulOptions = waveWSMP.fulOptions;
	m_rWSMP.usUnityNote = waveWSMP.usUnityNote;
}


CRegion * CRegion::GetNext() 
{
	return(CRegion *)AListItem::GetNext();
}

void CRegion::GetName()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_csName = GetRangeName(FALSE);
    m_csName += " to ";
    m_csName += GetRangeName(TRUE);
    
	if (m_pWave != NULL)
    {
        m_csName += " -> ";
		m_csName += m_pWave->GetName();
    }
}

CString CRegion::GetWaveName()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return m_pWave ? m_pWave->GetName() : "";
}

CString CRegion::GetRangeName(BOOL bUpper)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	char sBuffer[10]; // J3 make a define
	notetostring(GetRangeValue(bUpper), sBuffer);
    
    return CString(sBuffer);
}

USHORT CRegion::GetRangeValue(BOOL bUpper)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return bUpper ? m_rRgnHeader.RangeKey.usHigh : m_rRgnHeader.RangeKey.usLow;
}

void CRegion::SetRangeValue(BOOL bUpper, CString csName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if (bUpper)
    {
        m_rRgnHeader.RangeKey.usHigh = stringtonote(csName.GetBuffer(0));

    }
    else
    {
        m_rRgnHeader.RangeKey.usLow = stringtonote(csName.GetBuffer(0));
    }
    csName.ReleaseBuffer();
}

/////////////////////////////////////////////////////////////////////////////
// CRegion IUnknown implementation

HRESULT CRegion::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CRegion::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CRegion::Release()
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
// CRegion IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdNode::GetNodeImageIndex

HRESULT CRegion::GetNodeImageIndex(short* pnFirstImage)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pnFirstImage);
	
	ASSERT(m_pComponent != NULL);

	if(m_bConditionOK)
	{
		return(m_pComponent->GetRegionImageIndex(pnFirstImage));
	}
	else
	{
		return(m_pComponent->GetRegionGrayedImageIndex(pnFirstImage));
	}
}

/////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdNode::GetComponent

HRESULT CRegion::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT( m_pComponent != NULL );

	return m_pComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdNode::GetRootNode

HRESULT CRegion::GetDocRootNode( IDMUSProdNode** ppIRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIRootNode != NULL );

	m_pIRootNode->AddRef();
	*ppIRootNode = m_pIRootNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdNode::SetRootNode

HRESULT CRegion::SetDocRootNode( IDMUSProdNode* pIRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIRootNode != NULL );

	m_pIRootNode = pIRootNode;
//	m_pIRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdNode::GetParentNode

HRESULT CRegion::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdNode::SetParentNode

HRESULT CRegion::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdNode::GetNodeId

HRESULT CRegion::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_RegionNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdNode::GetNodeName

HRESULT CRegion::GetNodeName(BSTR* pbstrName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pbstrName);
    
 	GetName();
	*pbstrName = m_csName.AllocSysString();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdNode::GetNodeNameMaxLength

HRESULT CRegion::GetNodeNameMaxLength(short* pnMaxLength)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pnMaxLength);
	
	*pnMaxLength = -1; // Can't edit Region name.

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdNode::ValidateNodeName

HRESULT CRegion::ValidateNodeName(BSTR bstrName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Just free bstrName; can't rename a Region
	::SysFreeString(bstrName);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdNode::SetNodeName

HRESULT CRegion::SetNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName = bstrName;
	::SysFreeString( bstrName );

	m_csName = strName;

	// Set flag so we know to save file 
	m_pInstrument->m_pCollection->SetDirtyFlag();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdNode::GetEditorClsId

HRESULT CRegion::GetEditorClsId(CLSID* pClsId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(m_pInstrument);
	
	return(m_pInstrument->GetEditorClsId(pClsId));
}


/////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdNode::GetEditorTitle

HRESULT CRegion::GetEditorTitle(BSTR* pbstrTitle)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pInstrument);
	
	return(m_pInstrument->GetEditorTitle(pbstrTitle));
}


/////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdNode::GetEditorWindow

HRESULT CRegion::GetEditorWindow(HWND* hWndEditor)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pInstrument);
	
	if ( m_bDeletingRegion )
	{
		*hWndEditor = NULL;
		return S_OK;
	}
	else
		return(m_pInstrument->GetEditorWindow(hWndEditor));
}


/////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdNode::SetEditorWindow

HRESULT CRegion::SetEditorWindow(HWND hWndEditor)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pInstrument);
	
	return(m_pInstrument->SetEditorWindow(hWndEditor));
}


/////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdNode::UseOpenCloseImages

HRESULT CRegion::UseOpenCloseImages(BOOL* pfUseOpenCloseImages)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pfUseOpenCloseImages);

	*pfUseOpenCloseImages = TRUE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdNode::GetRightClickMenuId

HRESULT CRegion::GetRightClickMenuId(HINSTANCE* phInstance, UINT* pnMenuId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pnMenuId);
	ASSERT(phInstance);

	*phInstance = theApp.m_hInstance;
	*pnMenuId = IDM_REGION_NODE_RMENU;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdNode::OnRightClickMenuInit

HRESULT CRegion::OnRightClickMenuInit(HMENU hMenu)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	BOOL status;
	/*if(m_pArticulation)
	{//Note DLS 1 should be || !(m_pInstrument->m_rInstHeader.Locale.ulBank & F_INSTRUMENT_DRUMS)
		status = EnableMenuItem(hMenu, IDM_INSERT_ART, MF_BYCOMMAND | MF_GRAYED);
	}
	else
	{
		status = EnableMenuItem(hMenu, IDM_INSERT_ART, MF_BYCOMMAND | MF_ENABLED);
	}*/
	if (1 < m_pInstrument->m_Regions.GetCount())//can't delete only region
	{
		status = EnableMenuItem(hMenu, IDM_DELETE, MF_BYCOMMAND | MF_ENABLED);
	}
	else
	{
		status = EnableMenuItem(hMenu, IDM_DELETE, MF_BYCOMMAND | MF_GRAYED);
	}
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdNode::OnRightClickMenuSelect

HRESULT CRegion::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pComponent != NULL);
	ASSERT(m_pComponent->m_pIFramework != NULL);

	HRESULT hr = E_FAIL;
	IDMUSProdNode* pIParentNode;
	
	switch( lCommandId )
	{
		case IDM_OPEN:
			if(SUCCEEDED(m_pComponent->m_pIFramework->OpenEditor(this)))
			{
				ASSERT(m_pInstrument);
				ASSERT(m_pInstrument->GetInstrumentEditor());
				CInstrumentFVEditor* pIE;
				pIE = m_pInstrument->GetInstrumentEditor();
				if(pIE)
					pIE->SetCurRegion(this);

				hr = S_OK;
			}
			break;
		
		case IDM_INSERT_ART_LIST: 
			{
				if(m_pInstrument)
				{
					m_pInstrument->SaveStateForUndo(IDS_UNDO_INSERT_ARTLIST);
				}
				if(FAILED(InsertChildNode(NULL)))
				{
					return E_FAIL;
				}
				
				hr = S_OK;
				break;
			}
			
		case IDM_CUT:
			hr = E_NOTIMPL;			
			break;

		case IDM_COPY:
			hr = E_NOTIMPL;
			break;

		case IDM_PASTE:
			hr = E_NOTIMPL;
			break;

		case IDM_DELETE:
			if(SUCCEEDED(GetParentNode(&pIParentNode)))
			{
				if(m_pInstrument)
				{
					CInstrumentFVEditor* pIE;
					pIE = m_pInstrument->GetInstrumentEditor();
					if(pIE)
						pIE->TurnOffMidiNotes();
					else
					{
						m_pInstrument->Stop(true);
						m_pInstrument->TurnOffMidiNotes();
						if(m_pInstrument->m_pInstrumentCtrl)
							m_pInstrument->m_pInstrumentCtrl->TurnOffMidiNotes();
					}

				}

				m_bDeletingRegion = true;
				if(SUCCEEDED(pIParentNode->DeleteChildNode((IDMUSProdNode *)this, TRUE)))
				{
					hr = S_OK;				
				}
				pIParentNode->Release();
			}

			break;

		case IDM_RENAME:
			if(SUCCEEDED(m_pComponent->m_pIFramework->EditNodeLabel((IDMUSProdNode *)this)))
			{
				hr = S_OK;
			}
			break;
		
		case IDM_PROPERTIES:
			if(SUCCEEDED(OnShowProperties()))
			{
				hr = S_OK;
			}
			break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdNode::DeleteChildNode

HRESULT CRegion::DeleteChildNode(IDMUSProdNode* pIChildNode, BOOL fPromptUser)
{
 	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pIChildNode != NULL);
	ASSERT(m_pComponent != NULL);
	ASSERT(m_pComponent->m_pIFramework != NULL);

	HRESULT hr = S_OK;
	
	// Remove node from Project Tree
	if(m_pComponent->m_pIFramework->RemoveNode(pIChildNode, fPromptUser) == S_FALSE)
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
	
	m_pArticulation = NULL;

	if (m_pInstrumentFVEditor)
	{
		m_pInstrumentFVEditor->UpdateUseInstArt();
	}

	if(GetArticulationCount() == 0 && m_pInstrument->GetArticulationCount() == 0)
	{
		hr = m_pInstrument->InsertChildNode(NULL);
	}	

	// Set flag so we know to save file 
	m_pInstrument->m_pCollection->SetDirtyFlag();
	m_pInstrument->UpdateInstrument();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdNode::InsertChildNode

HRESULT CRegion::InsertChildNode(IDMUSProdNode* pIChildNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pComponent != NULL);
	ASSERT(m_pComponent->m_pIFramework != NULL);	
	ASSERT(m_pInstrument);

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

	// If we're inserting an articulation chances are we want to use it...
	m_lstArticulationLists.AddTail((CArticulationList*)pIChildNode);
	m_pArticulation = ((CArticulationList*)pIChildNode)->GetHead();
	m_bUseInstrumentArticulation = false;


	SetCurrentArtInEditor();		


	// Set root and parent node of ALL children
	theApp.SetNodePointers(pIChildNode, (IDMUSProdNode *)(m_pInstrument->m_pCollection), (IDMUSProdNode *)this);

	// Add node to Project Tree
	if( !SUCCEEDED (m_pComponent->m_pIFramework->AddNode(pIChildNode, (IDMUSProdNode *)this)))
	{
		DeleteChildNode( pIChildNode, FALSE );
		return E_FAIL;
	}

	// Set flag to indicate the DLS collection now needs to be saved
	m_pInstrument->m_pCollection->SetDirtyFlag();
	m_pInstrument->UpdateInstrument();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CRegion IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CRegion IPersist::GetClassID

HRESULT CRegion::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( pClsId != NULL );

    memset( pClsId, 0, sizeof( CLSID ) );

    return E_NOTIMPL;
}

////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdNode::DeleteNode

HRESULT CRegion::DeleteNode( BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	IDMUSProdNode* pIParentNode;

	if( SUCCEEDED ( GetParentNode(&pIParentNode) ) )
	{
		if( pIParentNode )
		{
			m_bDeletingRegion = true;
			HRESULT hr = pIParentNode->DeleteChildNode( this, fPromptUser );
			pIParentNode->Release();
			return hr;		
		}
	}

	return E_FAIL;
}

//////////////////////////////////////////////////////////////////////
//
//	CRegion IDMUSProdNode::GetFirstChild
//
//////////////////////////////////////////////////////////////////////
HRESULT CRegion::GetFirstChild(IDMUSProdNode** ppIFirstChildNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(ppIFirstChildNode == NULL)
	{
		return E_POINTER;
	}

	*ppIFirstChildNode = NULL;

	if(m_lstArticulationLists.GetCount() > 0)
	{
		CArticulationList* pArticList = (CArticulationList*)m_lstArticulationLists.GetHead();
		ASSERT(pArticList);
		IDMUSProdNode* pINode = static_cast<IDMUSProdNode*>(pArticList);

		if(pINode)
		{
			pINode->AddRef();
			*ppIFirstChildNode = pINode;
		}
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CRegion IDMUSProdNode::GetNextChild
//
//////////////////////////////////////////////////////////////////////
HRESULT CRegion::GetNextChild(IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode)
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

	CArticulationList* pArticList = dynamic_cast<CArticulationList*>(pIChildNode);
	if(pArticList)
	{
		POSITION position = m_lstArticulationLists.Find(pArticList);
		ASSERT(position);
		pArticList = m_lstArticulationLists.GetNext(position);
		if(position)
		{
			pArticList = (CArticulationList*) m_lstArticulationLists.GetNext(position);
			pArticList->AddRef();
			*ppINextChildNode = dynamic_cast<IDMUSProdNode*>(pArticList);
			return S_OK;
		}
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CRegion IDMUSProdNode::GetNodeListInfo
//
//////////////////////////////////////////////////////////////////////
HRESULT CRegion::GetNodeListInfo(DMUSProdListInfo* pListInfo)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pListInfo);
	
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdNode::OnNodeSelChanged

HRESULT CRegion::OnNodeSelChanged(BOOL fSelected)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pInstrument);
	ASSERT( m_pComponent != NULL );

	if(fSelected)
	{
		ASSERT( m_pComponent->m_pIFramework != NULL );
		
		if(m_pInstrument->m_pInstrumentCtrl)
		{
			CInstrumentFVEditor* pIE;
			pIE = m_pInstrument->GetInstrumentEditor();
			if(pIE)
			{
				pIE->SetCurRegion(this);

				// If Region has an Articulation we want to use it, if not we want to use the 
				// Instrument Articulation.
				if ( m_pArticulation && !m_bUseInstrumentArticulation )
				{
					pIE->SetCurArticulation(m_pArticulation);
				}
				else
				{
					pIE->SetCurArticulation(m_pInstrument->GetCurrentArticulation());		
				}
				pIE->SendRegionChange();
			}
			m_pInstrument->m_pComponent->m_pIConductor->SetActiveTransport(m_pInstrument, BS_PLAY_ENABLED | BS_NO_AUTO_UPDATE);

		}
	}	
	else
	{
		m_pInstrument->m_pComponent->m_pIConductor->TransportStopped(m_pInstrument);
		m_pInstrument->m_pComponent->m_pIConductor->SetBtnStates(m_pInstrument, BS_NO_AUTO_UPDATE);
		CInstrumentFVEditor* pIE;
		pIE = m_pInstrument->GetInstrumentEditor();
		if(pIE)
			pIE->TurnOffMidiNotes();
		else
		{
			m_pInstrument->TurnOffMidiNotes();
			m_pInstrument->Stop(true);
		}
	}

	if(m_pInstrument)
	{
		m_pInstrument->UpdateInstrument();
	}

	return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
//
// CRegion IDMUSProdNode::CreateDataObject
//
//////////////////////////////////////////////////////////////////////
HRESULT CRegion::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CRegion IDMUSProdNode::CanCut
//
//////////////////////////////////////////////////////////////////////
HRESULT CRegion::CanCut()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CRegion IDMUSProdNode::CanCopy
//
//////////////////////////////////////////////////////////////////////
HRESULT CRegion::CanCopy()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CRegion IDMUSProdNode::CanDelete
//
//////////////////////////////////////////////////////////////////////
HRESULT CRegion::CanDelete()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (1 < m_pInstrument->m_Regions.GetCount())//can't delete only region
	{
		return S_OK;
	}
    return E_FAIL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CRegion IDMUSProdNode::CanDeleteChildNode
//
//////////////////////////////////////////////////////////////////////
HRESULT CRegion::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CRegion IDMUSProdNode::CanPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CRegion::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CRegion IDMUSProdNode::PasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CRegion::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CRegion IDMUSProdNode::CanChildPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CRegion::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
									    BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CRegion IDMUSProdNode::ChildPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CRegion::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdNode::GetObject

HRESULT CRegion::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CRegion IDMUSProdPropPageObject::GetData

HRESULT CRegion::GetData(void** ppData)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(ppData);

	*ppData = this;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument IDMUSProdPropPageObject::SetData

HRESULT CRegion::SetData(void* pData)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pData);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CCollection IDMUSProdPropPageObject::OnRemoveFromPageManager

HRESULT CRegion::OnRemoveFromPageManager(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CCollection IDMUSProdPropPageObject::OnShowProperties

HRESULT CRegion::OnShowProperties(void)
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
		int nActivePage = (int)CRegionPropPgMgr::dwLastSelPage;
		
		if(SUCCEEDED(hr = pIPropSheet->SetPageManager(m_pRegionPropPgMgr)))
		{
			m_pRegionPropPgMgr->SetObject(this);
			pIPropSheet->SetActivePage(short(nActivePage)); 
		}

		pIPropSheet->Show(TRUE);
		pIPropSheet->Release();
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CRegion Additional functions

/////////////////////////////////////////////////////////////////////////////
// CRegion::Save

HRESULT CRegion::Save(IDMUSProdRIFFStream* pIRiffStream, bool fFullSave,
	USHORT usRegionSaved)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pIRiffStream);

	HRESULT			hr = S_OK;			// Return code
    MMCKINFO		ick;
    MMCKINFO		ck;
	DWORD			cb;
    DWORD           dwSaveSize;
	
    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
    if(pIStream == NULL)
        return E_OUTOFMEMORY;

	// decide what we'll save
	bool bSaveArticulationList = true;
	bool bSaveDesignChunks = true;
	IDMUSProdPersistInfo* pIJazzPersistInfo;
	if ( SUCCEEDED(pIStream->QueryInterface(IID_IDMUSProdPersistInfo,(void**) &pIJazzPersistInfo)) )
	{
		DMUSProdStreamInfo	StreamInfo;
		FileType	ftSaveType;

		pIJazzPersistInfo->GetStreamInfo(&StreamInfo);
		ftSaveType = StreamInfo.ftFileType;

		if (ftSaveType == FT_RUNTIME)
			{
			bSaveArticulationList = !m_bUseInstrumentArticulation && (GetArticulationCount() != 0);
			bSaveDesignChunks = false;
			}

		pIJazzPersistInfo->Release();
	}

	// save each channel
	LONG cChannels = GetChannelCount();
	for (LONG iChannel = 0; iChannel < cChannels; iChannel++)
		{
	    m_bDLS1Region = (m_nLayer == 0);
	    if (m_pWave && (m_pWave->GetChannelCount() > 1))
	    	m_bDLS1Region = false;
		ick.fccType = m_bDLS1Region ? FOURCC_RGN : FOURCC_RGN2;

	    hr = pIRiffStream->CreateChunk(&ick, MMIO_CREATELIST);
	    if (FAILED(hr))
			goto LReleaseStreamAndFail;
		
		// Save the conditional chunk
		if(m_pConditionalChunk)
		{
			m_pConditionalChunk->Save(pIRiffStream);
		}

	    ck.ckid = FOURCC_RGNH ;
	    hr = pIRiffStream->CreateChunk(&ck, 0);
	    if(FAILED(hr))
			goto LReleaseStreamAndFail;

	    hr = pIStream->Write((LPSTR) &m_rRgnHeader, sizeof(RGNHEADER), &cb);	
	    if(FAILED(hr) || cb != sizeof(RGNHEADER))
			goto LReleaseStreamAndFail;

		// write layer (optional)
		hr = pIStream->Write((LPSTR) &m_nLayer, sizeof(USHORT), &cb);
	    if(FAILED(hr) || cb != sizeof(USHORT))
			goto LReleaseStreamAndFail;
		
	    if(FAILED(pIRiffStream->Ascend(&ck, 0)))
	    {
	    	hr = E_FAIL;
			goto LReleaseStreamAndFail;
	    }

	    ck.ckid = FOURCC_WSMP ;
	    hr = pIRiffStream->CreateChunk(&ck, 0);
	    if(FAILED(hr))
			goto LReleaseStreamAndFail;

		dwSaveSize = sizeof(WSMPL);
	    if(m_rWSMP.cSampleLoops > 0)
	    {
	        dwSaveSize += sizeof(WLOOP);
	        m_rWSMP.cSampleLoops = 1;
	    }
	    
		hr = pIStream->Write((LPSTR) &m_rWSMP,dwSaveSize, &cb);	
	    if (FAILED(hr) || cb != dwSaveSize)
			goto LReleaseStreamAndFail;
	    if (FAILED(pIRiffStream->Ascend(&ck, 0)))
	    {
	    	hr = E_FAIL;
			goto LReleaseStreamAndFail;
	    }

		if (fFullSave)
		{
			if (m_pWave != NULL)
			{
				// save wavelink
				m_pWave->SetWaveLinkPerChannel(iChannel, &m_rWaveLink);

				ck.ckid = FOURCC_WLNK ;
				hr = pIRiffStream->CreateChunk(&ck, 0);
				if (FAILED(hr))
					goto LReleaseStreamAndFail;
				hr = pIStream->Write((LPSTR) &m_rWaveLink,sizeof(WAVELINK), &cb);	
				if (FAILED(hr) || cb != sizeof(WAVELINK))
					goto LReleaseStreamAndFail;

				if (FAILED(pIRiffStream->Ascend(&ck, 0)))
					goto LReleaseStreamAndFail;
			}
		}

		// save design-time stuff
		if (bSaveDesignChunks)
			{
			ck.ckid = DLS_REGION_EXTRA_CHUNK;
			hr = pIRiffStream->CreateChunk(&ck, 0);
			if (FAILED(hr))
				goto LReleaseStreamAndFail;
				
			hr = pIStream->Write((LPSTR) &m_bUseInstrumentArticulation,sizeof(bool), &cb);
			if (FAILED(hr) || cb != sizeof(bool))
				goto LReleaseStreamAndFail;
			
			hr = pIStream->Write((LPSTR) &m_bOverrideWaveFineTune, sizeof(bool), &cb);
			if(FAILED(hr) || cb != sizeof(bool))
				goto LReleaseStreamAndFail;

			hr = pIStream->Write((LPSTR)&(usRegionSaved), sizeof(USHORT), &cb);
			if (cb != sizeof(USHORT))
				hr = E_FAIL;
			if (FAILED(hr))
				goto LReleaseStreamAndFail;

			hr = pIRiffStream->Ascend(&ck, 0);
			if (FAILED(hr))
				goto LReleaseStreamAndFail;
			}

		// save articulation list
		if (bSaveArticulationList)
			{
			POSITION position = m_lstArticulationLists.GetHeadPosition();
			while(position)
				{
				CArticulationList* pArticList = (CArticulationList*) m_lstArticulationLists.GetNext(position);
				ASSERT(pArticList);
				if(pArticList)
					{
					hr = pArticList->Save(pIRiffStream);
					if(FAILED(hr)) 
						goto LReleaseStreamAndFail;
					}
				}
			}
		
	    // save unknown chunks
	    m_UnknownChunk.Save(pIRiffStream);
	    pIRiffStream->Ascend(&ick, 0);
		}

	pIStream->Release();
	ASSERT(SUCCEEDED(hr)); // we should be in failure case
    return hr;

LReleaseStreamAndFail:
	ASSERT(FAILED(hr)); // hr hasn't been set correctly
	pIStream->Release();
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CRegion::Load

HRESULT CRegion::Load( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		return E_POINTER;
	}

	ASSERT(pIRiffStream);
	if(pIRiffStream == NULL)
	{
		return E_POINTER;
	}

    IStream* pIStream = NULL;
	DWORD cb = 0;
	DWORD cSize = 0;
    HRESULT	hr = S_OK;
    bool bExtraChunkPresent = false; // extra chunk present, overrides default behavior

    pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream != NULL);
	if(pIStream == NULL)
	{
		return E_OUTOFMEMORY;
	}

	// Remove all previous articulation lists
	DeleteArticulationLists();

	MMCKINFO ck;
	ck.ckid = 0;
	ck.fccType = 0;
    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case FOURCC_CDL:
			{
				if(m_pConditionalChunk == NULL)
				{
					m_pConditionalChunk = new CConditionalChunk(m_pComponent);
				}

				if(FAILED(m_pConditionalChunk->Load(pIRiffStream, &ck)))
				{
					pIStream->Release();
					return E_FAIL;
				}
				break;
			}

			case FOURCC_RGNH :
			{
				if( ck.cksize < sizeof(RGNHEADER) )
				{
					cSize = ck.cksize;
				}
				else
				{
					cSize = sizeof(RGNHEADER);
				}
				hr = pIStream->Read( &m_rRgnHeader, cSize, &cb );
	            if( FAILED(hr) || cb != cSize )
				{
					pIStream->Release();
					return (hr);
				}
				
				// Read the layer value if we have it...
				if(m_bDLS1Region == false && ck.cksize > sizeof(RGNHEADER))
				{
					hr = pIStream->Read( &m_nLayer, sizeof(USHORT), &cb );
					if(FAILED(hr) || cb != sizeof(USHORT))
					{
						pIStream->Release();
						return (hr);
					}

				}
				
				// If it's a DLS1 region with broken velocity ranges...fix it...
				if(m_bDLS1Region)
				{
					// DLS1 regions are always in layer 0
					m_nLayer = 0;

					if(m_rRgnHeader.RangeVelocity.usLow == 0 && m_rRgnHeader.RangeVelocity.usHigh == 0)
					{
						m_rRgnHeader.RangeVelocity.usHigh = 127;
					}

					// Mark the collection as dirty
					if(m_pInstrument && m_pInstrument->m_pCollection)
					{
						m_pInstrument->m_pCollection->SetDirtyFlag();
					}
				}

				if(m_pInstrument)
				{
					if(m_pInstrument->m_Regions.GetNumberOfLayers() < m_nLayer)
					{
						m_pInstrument->m_Regions.SetNumberOfLayers(m_nLayer + 1);
					}
				}

				break;
			}

			case DLS_REGION_EXTRA_CHUNK :
			{
				bExtraChunkPresent = true;
				hr = pIStream->Read( &m_bUseInstrumentArticulation, sizeof(bool), &cb );
	            if( FAILED(hr) || cb != sizeof(bool))
				{
					pIStream->Release();
					return (hr);
				}

				hr = pIStream->Read(&m_bOverrideWaveFineTune, sizeof(bool), &cb);
				if(FAILED(hr) || cb != sizeof(bool))
				{
					pIStream->Release();
					return hr;
				}

				// IDs shared between multiple channels really representing the same region
				if (ck.cksize < sizeof(bool)+sizeof(bool)+sizeof(USHORT))
					break;
				hr = pIStream->Read(&m_usRegionSaved, sizeof(USHORT), &cb);
				if (cb != sizeof(USHORT))
					hr = E_FAIL;
				if (FAILED(hr))
					{
					pIStream->Release();
					return hr;
					}
				break;
			}

			case FOURCC_WSMP :
			{
				if( ck.cksize < (sizeof(WSMPL) + sizeof(WLOOP)) )
				{
					cSize = ck.cksize;
				}
				else
				{
					cSize = sizeof(WSMPL) + sizeof(WLOOP);
				}
				hr = pIStream->Read( &m_rWSMP, cSize, &cb );
                if( FAILED(hr) || cb != cSize )
				{
					pIStream->Release();
					return (hr);
				}
				break;
			}

			case FOURCC_WLNK :
			{
				if( ck.cksize < sizeof(WAVELINK) )
				{
					cSize = ck.cksize;
				}
				else
				{
					cSize = sizeof(WAVELINK);
				}
				hr = pIStream->Read( &m_rWaveLink, cSize, &cb );
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
					case FOURCC_LAR2 :
					case FOURCC_LART :
                    {
						BOOL bDLS1 = FALSE;
						if(ck.fccType == FOURCC_LART)
						{
							bDLS1 = TRUE;
						}

						CArticulationList* pArticList = new CArticulationList(m_pComponent, this);
						if(pArticList == NULL)
						{
							pIStream->Release();
							return E_OUTOFMEMORY;
						}
						
						pArticList->SetDLS1(bDLS1);
						if(FAILED(hr = pArticList->Load(pIRiffStream, &ck)))
						{
							pIStream->Release();
							return hr;
						}

						m_lstArticulationLists.AddTail(pArticList);

						// use region articulation, unless overriden (fix 48914)
						if (!bExtraChunkPresent)
							m_bUseInstrumentArticulation = false;

						break;
                    }
					
                    default:
					{
                        m_UnknownChunk.Load(pIRiffStream, &ck, TRUE);
                        break;
					}

				}   
				break;
			}

			// Ignore edit chunks while loading.
            case mmioFOURCC('e','d','i','t'):   
                    break;
            default:
                m_UnknownChunk.Load(pIRiffStream, &ck, FALSE);
                break;
		}   //switch (ckid)
        pIRiffStream->Ascend( &ck, 0 );
    }
	pIStream->Release();

	// Set the first articulation in the very first articulation list as
	// the current articulation for this region...
	if(!m_lstArticulationLists.IsEmpty())
	{
		CArticulationList* pArtList = (CArticulationList*) m_lstArticulationLists.GetHead();
		ASSERT(pArtList);
		if(pArtList)
			m_pArticulation = pArtList->GetHead();
	}

	// Check WAVELINK.ulChannel field; make it 1 if 0....
	if(m_rWaveLink.ulChannel == 0)
		m_rWaveLink.ulChannel = WAVELINK_CHANNEL_LEFT;

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CRegion IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
//
// CRegion IPersistStream::IsDirty
//
//////////////////////////////////////////////////////////////////////
HRESULT CRegion::IsDirty()
{
    // I want to know if I am called
	ASSERT(FALSE);

	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CRegion IPersistStream::GetSizeMax
//
//////////////////////////////////////////////////////////////////////
HRESULT CRegion::GetSizeMax( ULARGE_INTEGER FAR* /*pcbSize*/ )
{
    // I want to know if I am called
	ASSERT(FALSE);

	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CRegion IPersistStream::Load
//
//////////////////////////////////////////////////////////////////////
HRESULT CRegion::Load(IStream* pIStream)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_POINTER;
	}

	ULONG ulBytesRead = 0;

	// Check the number of articulation lists in the stream
	int nArtListCount = 0;
	HRESULT hr = pIStream->Read((void *)&nArtListCount, sizeof(int), &ulBytesRead);
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

	// The RGNHEADER structure
	hr = pIStream->Read((void *)&m_rRgnHeader, sizeof(RGNHEADER), &ulBytesRead);
	if(FAILED(hr) || ulBytesRead != sizeof(RGNHEADER)) 
	{
		return E_FAIL;
	}

	// Read the layer info
	hr = pIStream->Read((void *)&m_nLayer, sizeof(USHORT), &ulBytesRead);
	if(FAILED(hr) || ulBytesRead != sizeof(USHORT)) 
	{
		return E_FAIL;
	}

	// The WAVELINK structure
	hr = pIStream->Read((void *)&m_rWaveLink, sizeof(WAVELINK), &ulBytesRead);
	if(FAILED(hr) || ulBytesRead != sizeof(WAVELINK)) 
	{
		return E_FAIL;
	}
	
	GetWave();

	// The WLOOP structure
	hr = pIStream->Read((void *)&m_rWLOOP, sizeof(WLOOP), &ulBytesRead);
	if(FAILED(hr) || ulBytesRead != sizeof(WLOOP)) 
	{
		return E_FAIL;
	}

	// The Region's WSMPL structure
	hr = pIStream->Read((void *)&m_rWSMP, sizeof(WSMPL), &ulBytesRead);
	if(FAILED(hr) || ulBytesRead != sizeof(WSMPL)) 
	{
		return E_FAIL;
	}

	// The length of the region name 
	int nCount = 0;
	hr = pIStream->Read((void *)&nCount, sizeof(int), &ulBytesRead);
	if(FAILED(hr) || ulBytesRead != sizeof(int)) 
	{
		return E_FAIL;
	}

	// Allocate and read the name
	CString tempName; 
	char* tempNameBuffer = tempName.GetBufferSetLength(nCount);
	hr = pIStream->Read((void *)tempNameBuffer, nCount, &ulBytesRead);
	if(FAILED(hr) || ulBytesRead != (ULONG) nCount) 
	{
		return E_FAIL;
	}
	tempName. ReleaseBuffer(nCount);
	m_csName = tempName;

	// Use Instrument Articulation flag
	hr = pIStream->Read((void *)&m_bUseInstrumentArticulation, sizeof(BOOL), &ulBytesRead);
	if(FAILED(hr) || ulBytesRead != sizeof(BOOL)) 
	{
		return E_FAIL;
	}

	
	/* - : Read the flags marked on the Region's Property page : - */
	
	// Is Attenuation Overridden?
	hr = pIStream->Read((void *)&m_bOverrideWaveAttenuation, sizeof(bool), &ulBytesRead);
	if(FAILED(hr) || ulBytesRead != sizeof(bool)) 
	{
		return E_FAIL;
	}

	// Is FineTune Overridden?
	hr = pIStream->Read((void *)&m_bOverrideWaveFineTune, sizeof(bool), &ulBytesRead);
	if(FAILED(hr) || ulBytesRead != sizeof(bool)) 
	{
		return E_FAIL;
	}

	// Is RootNote Overridden?
	hr = pIStream->Read((void *)&m_bOverrideWaveRootNote, sizeof(bool), &ulBytesRead);
	if(FAILED(hr) || ulBytesRead != sizeof(bool)) 
	{
		return E_FAIL;
	}

	// Is the region overriding the wave settings?
	hr = pIStream->Read((void *)&m_bOverrideWaveMoreSettings, sizeof(bool), &ulBytesRead);
	if(FAILED(hr) || ulBytesRead != sizeof(bool)) 
	{
		return E_FAIL;
	}

	// The length of the condition expression
	nCount = 0;
	hr = pIStream->Read((void *)&nCount, sizeof(int), &ulBytesRead);
	if(FAILED(hr) || ulBytesRead != sizeof(int)) 
	{
		return E_FAIL;
	}

	// Allocate and read the expression
	if(nCount > 0)
	{
		CString conditionName; 
		char* conditionNameBuffer = conditionName.GetBufferSetLength(nCount);
		hr = pIStream->Read((void *)conditionNameBuffer, nCount, &ulBytesRead);
		if(FAILED(hr) || ulBytesRead != (ULONG) nCount) 
		{
			return E_FAIL;
		}
		conditionName. ReleaseBuffer(nCount);
		m_pConditionalChunk->RegularToRPN(conditionName);
	}
	else
	{
		CString sNone;
		sNone.LoadString(IDS_NONE);
		m_pConditionalChunk->RegularToRPN(sNone);
	}


	// Was this region previously deleted?
	hr = pIStream->Read((void *)&m_bLastDeleted, sizeof(bool), &ulBytesRead);
	if (!SUCCEEDED(hr) || ulBytesRead != sizeof(bool)) 
	{
		return E_FAIL;
	}
	
	return hr; 
}

/////////////////////////////////////////////////////////////////////////////
//
// CRegion IPersistStream::Save
//
//////////////////////////////////////////////////////////////////////
HRESULT CRegion::Save(IStream* pIStream, BOOL fClearDirty)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_POINTER;
	}

	ULONG ulBytesWritten = 0;

	// If there is no global articulation we have to note that on the  stream
	int nCount = GetArticulationCount() ? 1 : 0;
	int nArtListCount = m_lstArticulationLists.GetCount();
	HRESULT hr = pIStream->Write((void*)&nArtListCount, sizeof(int), &ulBytesWritten);
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
    	
	// The RGNHEADER structure
	hr = pIStream->Write((void *)&m_rRgnHeader, sizeof(RGNHEADER), &ulBytesWritten);
	if(FAILED(hr) || ulBytesWritten != sizeof(RGNHEADER)) 
	{
		return E_FAIL;
	}

	// Save the layer info
	hr = pIStream->Write((void *)&m_nLayer, sizeof(USHORT), &ulBytesWritten);
	if(FAILED(hr) || ulBytesWritten != sizeof(USHORT)) 
	{
		return E_FAIL;
	}

	// THE WAVELINK structure
	hr = pIStream->Write((void *)&m_rWaveLink, sizeof(WAVELINK), &ulBytesWritten);
	if(FAILED(hr) || ulBytesWritten != sizeof(WAVELINK)) 
	{
		return E_FAIL;
	}

	// The WLOOP structure
	hr = pIStream->Write((void *)&m_rWLOOP, sizeof(WLOOP), &ulBytesWritten);
	if(FAILED(hr) || ulBytesWritten != sizeof(WLOOP)) 
	{
		return E_FAIL;
	}

	// The Region's WSMPL structure
	hr = pIStream->Write((void *)&m_rWSMP, sizeof(WSMPL), &ulBytesWritten);
	if(FAILED(hr) || ulBytesWritten != sizeof(WSMPL)) 
	{
		return E_FAIL;
	}

	// The region name length
	nCount = m_csName.GetLength();
	hr = pIStream->Write((void *)&nCount, sizeof(int), &ulBytesWritten);
	if(FAILED(hr) || ulBytesWritten != sizeof(int)) 
	{
		return E_FAIL;
	}

	// The Region Name
	hr = pIStream->Write((void *)(m_csName.GetBuffer(0)), nCount, &ulBytesWritten);
	if(FAILED(hr) || ulBytesWritten != (ULONG) nCount) 
	{
		return E_FAIL;
	}

	// Use Instrument Articulation flag
	hr = pIStream->Write((void *)(&m_bUseInstrumentArticulation), sizeof(BOOL), &ulBytesWritten);
	if(FAILED(hr) || ulBytesWritten != sizeof(BOOL)) 
	{
		return E_FAIL;
	}
	
	
	/* - : Save the flags marked on the regions property page : - */

	// Is Attenuation overridden?
	hr = pIStream->Write((void *)(&m_bOverrideWaveAttenuation), sizeof(bool), &ulBytesWritten);
	if(FAILED(hr) || ulBytesWritten != sizeof(bool)) 
	{
		return E_FAIL;
	}

	// Is Fine Tune overridden?
	hr = pIStream->Write((void *)(&m_bOverrideWaveFineTune), sizeof(bool), &ulBytesWritten);
	if(FAILED(hr) || ulBytesWritten != sizeof(bool)) 
	{
		return E_FAIL;
	}

	// Is Root Note overridden?
	hr = pIStream->Write((void *)(&m_bOverrideWaveRootNote), sizeof(bool), &ulBytesWritten);
	if(FAILED(hr) ||ulBytesWritten != sizeof(bool)) 
	{
		return E_FAIL;
	}

	// Does this region override the wave settings
	hr = pIStream->Write((void *)(&m_bOverrideWaveMoreSettings), sizeof(bool), &ulBytesWritten);
	if(FAILED(hr) || ulBytesWritten != sizeof(bool)) 
	{
		return E_FAIL;
	}

	// Save the condition
	CString sCondition = m_pConditionalChunk->GetCondition();
	nCount = sCondition .GetLength();
	hr = pIStream->Write((void *)&nCount, sizeof(int), &ulBytesWritten);
	if(FAILED(hr) || ulBytesWritten != sizeof(int)) 
	{
		return E_FAIL;
	}

	// The Condition Expression
	if(nCount > 0)
	{
		hr = pIStream->Write((void *)(sCondition .GetBuffer(0)), nCount, &ulBytesWritten);
		if(FAILED(hr) || ulBytesWritten != (ULONG) nCount) 
		{
			return E_FAIL;
		}
	}
	
	// Are we saving an undo state for a region delete action?
	// This will load up into the "m_bLastDeleted" flag....
	hr = pIStream->Write((void *)(&m_bDeletingRegion), sizeof(bool), &ulBytesWritten);
	if(FAILED(hr) || ulBytesWritten != sizeof(bool)) 
	{
		return E_FAIL;
	}

	return hr; 
}

HRESULT CRegion::Write(void* pv,
					   LONG iChannel,
					   DWORD* pdwCurOffset,
					   DWORD* pDMIOffsetTable,
					   DWORD* pdwCurIndex,
					   DWORD  dwIndexNextRegion,
                       IDirectMusicPortDownload* pIDMPortDownLoad)
{
    ASSERT(pIDMPortDownLoad);
    if(pIDMPortDownLoad == NULL)
    {
        return E_POINTER;
    }

	HRESULT hr = S_OK;
	if(!m_bConditionOK)
	{
		return hr;
	}

	DWORD dwRelativeCurOffset = 0;
	DMUS_REGION*	pRegion = (DMUS_REGION*)pv;
	// Argument validation - Debug
	ASSERT(pv);
	ASSERT(pdwCurOffset);
	ASSERT(pDMIOffsetTable);
	ASSERT(pdwCurIndex);

	__try
	{
		CopyMemory(pv, (void *)&m_rRgnHeader, sizeof(RGNHEADER));
		WAVELINK wl = m_rWaveLink;
		if (m_pWave)
		{
			m_pWave->SetWaveLinkPerChannel(iChannel, &wl);
			CMonoWave *pMonoWave = m_pWave->GetChannel(iChannel);
			ASSERT(pMonoWave);
			wl.ulTableIndex = pMonoWave->GetDownloadID(pIDMPortDownLoad);
		}
		pRegion->WaveLink = wl;
		pRegion->WSMP = m_rWSMP;
		pRegion->WLOOP[0] = m_rWLOOP;
		pRegion->ulNextRegionIdx = dwIndexNextRegion;

		(*pdwCurIndex)++;
		*pdwCurOffset += CHUNK_ALIGN(sizeof(DMUS_REGION));
		dwRelativeCurOffset += CHUNK_ALIGN(sizeof(DMUS_REGION));
		
		// Write extension chunks
		CUnknownChunkItem* pExtChk = m_UnknownChunk.GetHead();
		if(pExtChk)
		{
			((DMUS_REGION*)pv)->ulFirstExtCkIdx = *pdwCurIndex;
		}
		else
		{
			// If no extension chunks set to zero
			((DMUS_REGION*)pv)->ulFirstExtCkIdx = 0;
		}

		for(; pExtChk && SUCCEEDED(hr); pExtChk =(CUnknownChunkItem*) pExtChk->GetNext())
		{
			DWORD dwCountExtChk = m_UnknownChunk.GetCount();
			DWORD dwIndexNextExtChk = 0;

			if(dwCountExtChk == 1)
			{
				dwIndexNextExtChk = 0;
			}
			else
			{
				dwIndexNextExtChk = *pdwCurIndex + 1;
			}
			
			pDMIOffsetTable[*pdwCurIndex] = *pdwCurOffset;
			hr = pExtChk->Write(((BYTE *)pv + dwRelativeCurOffset), 
								pdwCurOffset,
								dwIndexNextExtChk);
			
			dwCountExtChk--;
			(*pdwCurIndex)++;
		}
		
		if(SUCCEEDED(hr))
		{
			// Write region articulation if we have one
			UINT nArtCount = GetArticulationCount();
			if(nArtCount > 0 && !m_bUseInstrumentArticulation)
			{
				POSITION position = m_lstArticulationLists.GetHeadPosition();
				while(position)
				{
					CArticulationList* pArtList = (CArticulationList*)m_lstArticulationLists.GetNext(position);
					if(pArtList && pArtList->IsConditionOK() == TRUE && pArtList->GetCount() > 0)
					{
						((DMUS_REGION*)pv)->ulRegionArtIdx = *pdwCurIndex;

						if(FAILED(hr = pArtList->Write((BYTE*)pv, &dwRelativeCurOffset, pdwCurOffset, pDMIOffsetTable, pdwCurIndex)))
						{
							break;
						}
					}
				}
			}
			else
			{
				((DMUS_REGION*)pv)->ulRegionArtIdx = 0;
			}
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		hr = E_UNEXPECTED;
	}
	return hr;
}

DWORD CRegion::Size()
{
	DWORD dwSize = 0;

	if(!m_bConditionOK)
	{
		return dwSize;
	}

	__try
	{
		dwSize += CHUNK_ALIGN(sizeof(DMUS_REGION));

		if(GetArticulationCount() > 0)
		{
			POSITION position = m_lstArticulationLists.GetHeadPosition();
			while(position)
			{
				CArticulationList* pArtList = (CArticulationList*) m_lstArticulationLists.GetNext(position);
				ASSERT(pArtList);
				if(pArtList)
				{
					dwSize += pArtList->Size();
				}
			}
		}

		// Calculate the space need for Region's extension chunks
		CUnknownChunkItem* pExtChk = m_UnknownChunk.GetHead();
		for(; pExtChk; pExtChk = (CUnknownChunkItem*) pExtChk->GetNext())
		{
			dwSize += pExtChk->Size();
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		dwSize = 0;
	}
	return dwSize;
}

DWORD CRegion::Count()
{
	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
	{
		return 0;
	}

	// Return nothing if we don't want to be downloaded
	if(!m_bConditionOK)
	{
		return 0;
	}

	bool bAuditionMode = m_pInstrument->GetAuditionMode();
	if(bAuditionMode == AUDITION_SOLO)
	{
		// Use the audition mode to update ONLY if the editor's open!
		CInstrumentFVEditor* pEditor = m_pInstrument->GetInstrumentEditor();
		if(pEditor != NULL)
		{
			USHORT usActiveLayer = pEditor->GetActiveLayer();
			if(usActiveLayer != m_nLayer)
			{
				return 0;
			}
		}
	}

	DWORD dwCount = 1;
	if(GetArticulationCount() > 0 && !m_bUseInstrumentArticulation)
	{
		dwCount += m_UnknownChunk.GetCount();
		POSITION position = m_lstArticulationLists.GetHeadPosition();
		while(position)
		{
			CArticulationList* pArtList = (CArticulationList*) m_lstArticulationLists.GetNext(position);
			ASSERT(pArtList);
			if(pArtList)
			{
				dwCount += pArtList->Count();
			}
		}

		return dwCount;
	}
	else
	{
		return m_UnknownChunk.GetCount() + dwCount;
	}
}

bool CRegion::ValidateUIA()
{
	// In our design files we save out the state of the "Use Instrument Articulation" checkbox in a 
	// special chunk. If we load files that do not have this chunk we need to check that if
	// m_bUseInstrumentArticulation is true, as initialized, the instrument does have an articulation
	if ( m_bUseInstrumentArticulation )
	{
		if (m_pInstrument->GetCurrentArticulation() == NULL)
		{
			m_bUseInstrumentArticulation = false;
			if (m_pArticulation == NULL)
			{
				//m_bUseInstrumentArticulation = true;
				return false;
			}
		}
	}
	else
	{
		if (m_pArticulation == NULL)
		{
			m_bUseInstrumentArticulation = true;
			if (m_pInstrument->GetCurrentArticulation() == NULL)
			{
				return false;
			}
		}
	}
	return true;
}

void CRegion::GetWave()
{
	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
		return;

	CPtrList waveList;
	if(FAILED(m_pInstrument->m_pCollection->m_Waves.GetListOfWaves(&waveList)))
		return;
	
	POSITION position = waveList.GetHeadPosition();
	while (position)
		{
		CWave* pWave = (CWave*) waveList.GetNext(position);
		ASSERT(pWave);
		
		if ((pWave != NULL) && (m_pWave != pWave))
			{
			if (pWave->GetWaveForID(m_rWaveLink.ulTableIndex) ||
				((pWave->GetChannelCount() == 1) && ((CMonoWave *)pWave)->GetWaveForDownloadID(m_rWaveLink.ulTableIndex))) // if mono wave, also check download port
				{
				m_pWave->GetNode()->Release();
				m_pWave = pWave;
				pWave->GetNode()->AddRef();
				break;
				}
			}
		}
}

void CRegion::SetWave(CWave* pWave)
{
	ASSERT(pWave);
	if(pWave == NULL)
		return;

	// Reset multichannel settings only if they can't be retained (fix 51314)
	if ((m_pWave == NULL) ||(pWave->GetChannelCount() != m_pWave->GetChannelCount()))
		{
		m_rWaveLink.fusOptions = (pWave->GetChannelCount() > 1) ? F_WAVELINK_MULTICHANNEL : 0;
		m_rWaveLink.ulChannel = 1;
		}

	// Release the previous wave
	if(m_pWave)
	{
		CWaveNode* pWaveNode = m_pWave->GetNode();
		// Release the current wave
		if(pWaveNode)
		{
			pWaveNode->Release();
		}
	}

	// Assign the new wave
	m_pWave = pWave;
	CWaveNode* pWaveNode = pWave->GetNode();
	if(pWaveNode)
		pWaveNode->AddRef();

	m_rWaveLink.ulTableIndex = pWave->GetWaveID();

	WSMPL waveWSMPL = pWave->GetWSMPL();
	WLOOP waveWLOOP = pWave->GetWLOOP();

	if(m_bOverrideWaveAttenuation)
	{
		waveWSMPL.lAttenuation = m_rWSMP.lAttenuation;
	}

	if(m_bOverrideWaveFineTune)
	{
		waveWSMPL.sFineTune = m_rWSMP.sFineTune;
	}

	if(m_bOverrideWaveRootNote)
	{
		waveWSMPL.usUnityNote = m_rWSMP.usUnityNote;
	}

	if(m_bOverrideWaveMoreSettings)
	{
		memcpy(&waveWLOOP, &m_rWLOOP, sizeof(waveWLOOP));
	}

	// Copy wave sample info to region
	memcpy(&m_rWSMP, &waveWSMPL, sizeof(m_rWSMP));
	memcpy(&m_rWLOOP, &waveWLOOP, sizeof(m_rWLOOP));

	// update names
    GetName();
	if (m_pInstrumentFVEditor)
		m_pInstrumentFVEditor->m_RegionKeyBoard.SetWaveName(short(GetLayer()), GetRangeValue(false), GetWaveName());
}


int CRegion::GetLayer()
{
	return m_nLayer;
}

void CRegion::SetLayer(int nLayer)
{
	m_nLayer = USHORT(nLayer);
	m_bDLS1Region = (m_nLayer == 0);
	if ((m_pWave != NULL) && (m_pWave->GetChannelCount() > 1))
		m_bDLS1Region = false;
	
	if(m_pInstrument)
	{
		if(m_pInstrument->m_Regions.GetNumberOfLayers() < m_nLayer)
		{
			m_pInstrument->m_Regions.SetNumberOfLayers(m_nLayer);
		}
	}
}

CRegion* CRegion::IsOverlappingRegion(CRegion* pRegion)
{
	// We need a region to check...
	if(pRegion == NULL)
		return NULL;

	// We don't care about non-DLS1 regions...they can overlap...
	if(m_bDLS1Region == false)
		return NULL;

	// Will always overlap with self huh?
	if(pRegion == this)
		return NULL;

	USHORT usStartNote = pRegion->m_rRgnHeader.RangeKey.usLow;
	USHORT usEndNote = pRegion->m_rRgnHeader.RangeKey.usHigh;

	return IsOverlappingRegion(usStartNote, usEndNote);
}

CRegion* CRegion::IsOverlappingRegion(USHORT usStartNote, USHORT usEndNote)
{
	// We don't care about non-DLS1 regions...they can overlap...
	if(m_bDLS1Region == false)
		return NULL;

	if(usStartNote >= m_rRgnHeader.RangeKey.usLow && usStartNote <= m_rRgnHeader.RangeKey.usHigh)
		return this;

	if(usEndNote >= m_rRgnHeader.RangeKey.usLow && usEndNote <= m_rRgnHeader.RangeKey.usHigh)
		return this;

	if(m_rRgnHeader.RangeKey.usLow >= usStartNote && m_rRgnHeader.RangeKey.usLow <= usEndNote)
		return this;

	if(m_rRgnHeader.RangeKey.usHigh >= usStartNote && m_rRgnHeader.RangeKey.usHigh <= usEndNote)
		return this;

	return NULL;
}

CConditionalChunk* CRegion::GetConditionalChunk()
{
	return m_pConditionalChunk;
}

HRESULT	CRegion::SetCondition(const CString& sCondition)
{
	ASSERT(m_pConditionalChunk);
	if(m_pConditionalChunk == NULL)
		return E_FAIL;

	return m_pConditionalChunk->RegularToRPN(sCondition);
}

UINT CRegion::GetArticulationCount()
{
	UINT nRegionArticulations = 0;

	POSITION position = m_lstArticulationLists.GetHeadPosition();
	while(position)
	{
		CArticulationList* pArticList = (CArticulationList*) m_lstArticulationLists.GetNext(position);
		ASSERT(pArticList);
		if(pArticList)
		{
			nRegionArticulations += pArticList->GetCount();
		}
	}

	return nRegionArticulations;
}


// Returns the number of articulations that will actually get downloaded
UINT CRegion::GetArticulationsUsed()
{
	UINT nRegionArticulations = 0;

	POSITION position = m_lstArticulationLists.GetHeadPosition();
	while(position)
	{
		CArticulationList* pArticList = (CArticulationList*) m_lstArticulationLists.GetNext(position);
		ASSERT(pArticList);
		if(pArticList)
		{
			nRegionArticulations += pArticList->GetArticulationsUsed();
		}
	}

	return nRegionArticulations;
}


void CRegion::SetCurrentArticulation(CArticulation* pArticulation)
{
	// We don't check for NULL as we might be doing it 
	// intentionally when deleting a local articulation...
	m_pArticulation = pArticulation;

	if(m_pArticulation)
	{
		SetCurrentArtInEditor();
	}
}


void CRegion::DeleteArticulationLists()
{
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

	// This articulation is already pfft!
	m_pArticulation = NULL;
}


CArticulation* CRegion::GetCurrentArticulation()
{
	return m_pArticulation;
}


void CRegion::SetCurrentArtInEditor()
{
	// We don't have an articulation??
	if(m_pArticulation == NULL)
	{
		if(GetArticulationCount() == 0)
		{
			InsertChildNode(NULL);
		}
		else
		{
			CArticulationList* pArticList = m_lstArticulationLists.GetHead();
			ASSERT(pArticList);

			m_pArticulation = pArticList->GetHead();
		}
	}

	if(m_pInstrument && m_pInstrument->m_pInstrumentCtrl)
	{
		CInstrumentFVEditor* pIE = m_pInstrument->GetInstrumentEditor();
		if(pIE)
		{
			pIE->SetCurRegion(this);
			pIE->SendRegionChange();
			pIE->SetCurArticulation(m_pArticulation);
		}
	}
}


HRESULT CRegion::GetListOfConditions(CPtrList* pConditionList)
{
	ASSERT(pConditionList);
	if(pConditionList == NULL)
	{
		return E_POINTER;
	}

	ASSERT(m_pConditionalChunk);
	if(m_pConditionalChunk)
	{
		pConditionList->AddTail(m_pConditionalChunk);
	}

	POSITION position = m_lstArticulationLists.GetHeadPosition();
	while(position)
	{
		CArticulationList* pArtList = (CArticulationList*)m_lstArticulationLists.GetNext(position);
		ASSERT(pArtList);
		if(pArtList)
		{
			CConditionalChunk* pConditionalChunk = pArtList->GetConditionalChunk();
			ASSERT(pConditionalChunk);
			if(pConditionalChunk)
			{
				pConditionList->AddTail(pConditionalChunk);
			}
		}
	}

	return S_OK;
}


void CRegion::OnConditionConfigChanged(CSystemConfiguration* pCurrentConfig, bool bRefreshNode)
{
	ASSERT(pCurrentConfig);
	if(pCurrentConfig == NULL)
	{
		return;
	}

	m_bConditionOK = TRUE;

	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		m_bConditionOK = FALSE;
	}

	ASSERT(m_pComponent->m_pIFramework);
	if(m_pComponent->m_pIFramework == NULL)
	{
		m_bConditionOK = FALSE;
	}
	
	IDMUSProdFramework* pIFramework = m_pComponent->m_pIFramework;
	ASSERT(m_pConditionalChunk);
	if(m_pConditionalChunk == NULL)
	{
		m_bConditionOK = FALSE;
	}

	// We need to evaluate the condition defined in the conditional chunk 
	// from the values set in the configuration and decide on the download
	m_bConditionOK = m_pConditionalChunk->Evaluate(pCurrentConfig);
	CInstrumentFVEditor* pIE = m_pInstrument->GetInstrumentEditor();
	if(pIE)
	{
		pIE->m_RegionKeyBoard.EnableRegion(short(GetLayer()), m_rRgnHeader.RangeKey.usLow, m_bConditionOK);
	}

	// If the region doesn't go down; no articulations go doen either....
	POSITION position = m_lstArticulationLists.GetHeadPosition();
	while(position)
	{
		CArticulationList* pArtList = (CArticulationList*) m_lstArticulationLists.GetNext(position);
		ASSERT(pArtList);
		if(pArtList)
		{
			if(m_bConditionOK == FALSE)
			{
				pArtList->SetConditionOK(m_bConditionOK, bRefreshNode);
			}
			else
			{
				pArtList->OnConditionConfigChanged(pCurrentConfig, bRefreshNode);
			}
		}
	}

	if(bRefreshNode)
	{
		pIFramework->RefreshNode(this);
	}
}


void CRegion::CheckConfigAndRefreshNode()
{
	ASSERT(m_pConditionalChunk);
	if(m_pConditionalChunk == NULL)
	{
		m_bConditionOK = FALSE;
	}

	CSystemConfiguration* pCurrentConfig = m_pComponent->GetCurrentConfig();
	OnConditionConfigChanged(pCurrentConfig, true);
	if(m_pInstrument)
	{
		m_pInstrument->UpdateInstrument();
	}
}

void CRegion::CheckArtsAndSetInstrumentArt()
{
	// Set the use instrument art flag if we have no local arts that will go down
	// Else the synth complains
	if(GetArticulationsUsed() == 0)
	{
		m_bUseInstrumentArticulation = true;
		if(m_pInstrument)
		{
			CInstrumentFVEditor* pIE = m_pInstrument->GetInstrumentEditor();
			if(pIE)
			{
				pIE->UpdateUseInstArt();
			}
		}
	}
}

HRESULT	CRegion::CreateAndInsertArticulationList()
{
	return InsertChildNode(NULL);
}


HRESULT	CRegion::CopyProperties(CRegion* pRegion)
{
	ASSERT(pRegion);
	if(pRegion == NULL)
	{
		return E_POINTER;
	}
	
	ASSERT(pRegion->m_pWave);
	if(pRegion->m_pWave == NULL)
	{
		return E_UNEXPECTED;
	}

	m_rRgnHeader = pRegion->m_rRgnHeader;
	m_rWaveLink = pRegion->m_rWaveLink;
	m_rWSMP = pRegion->m_rWSMP;
	m_rWLOOP = pRegion->m_rWLOOP;
	m_bOverrideWaveAttenuation = pRegion->m_bOverrideWaveAttenuation;
	m_bOverrideWaveFineTune = pRegion->m_bOverrideWaveFineTune;
	m_bOverrideWaveRootNote = pRegion->m_bOverrideWaveRootNote;
	m_bOverrideWaveMoreSettings = pRegion->m_bOverrideWaveMoreSettings;

	// Set the condition to be the same
	CConditionalChunk* pConditionalChunk = pRegion->GetConditionalChunk();
	CString sCondition = pConditionalChunk->GetCondition();
	SetCondition(sCondition);

	// Need to update region keyboard with new region range, otherwise update in SetWave() will fail
	if (m_pInstrumentFVEditor)
		m_pInstrumentFVEditor->m_RegionKeyBoard.SetRange(m_rRgnHeader.RangeKey.usLow, m_rRgnHeader.RangeKey.usHigh, m_rRgnHeader.RangeVelocity.usLow, m_rRgnHeader.RangeVelocity.usHigh);

	CWave* pWave = pRegion->m_pWave;
	ASSERT(pWave);
	if(pWave == NULL)
	{
		return E_UNEXPECTED;
	}

	SetWave(pWave);
	CheckConfigAndRefreshNode();

	return S_OK;
}	

WAVELINK CRegion::GetWaveLink()
{
	return m_rWaveLink;
}

void CRegion::SetWaveLink(WAVELINK waveLink)
{
	CopyMemory(&m_rWaveLink, &waveLink, sizeof(WAVELINK));
}

bool CRegion::IsOkayToPlay(USHORT usActiveLayer, USHORT usNote, USHORT usVelocity, bool bIgnoreLayer)
{
	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
	{
		return false;
	}

	if(bIgnoreLayer == false)
	{
		if(usActiveLayer != m_nLayer)
		{
			return false;
		}
	
	}

	if(usNote < m_rRgnHeader.RangeKey.usLow || usNote > m_rRgnHeader.RangeKey.	usHigh)
	{
		return false;
	}

	if(usVelocity < m_rRgnHeader.RangeVelocity.usLow || usVelocity > m_rRgnHeader.RangeVelocity.usHigh)
	{
		return false;
	}

	return true;
}


CWaveNode* CRegion::GetWaveNode()
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return NULL;

	return m_pWave->GetNode();
}

/* returns the number of channels in the wave pointed to by this region */
LONG CRegion::GetChannelCount()
{
	if (m_pWave == NULL)
		{
		ASSERT(FALSE);
		return 0;
		}

	return m_pWave->GetChannelCount();
}


/////////////////////////////////////////////////////////////////////////////
// MergeNextRegions

/* merges following regions that have the same m_usRegionSaved, with this one, and returns the
	"master" wave that should be used (regions with same saved ID are supposed to
	point to different channels of the same wave) */
void CRegion::MergeNextRegions(CWave* &pWave)
{
	// if there are regions further in the list with the same ID, merge them with this one
	if (m_usRegionSaved > 0)
		{
		CRegion *pRegionPrev = this;
		CRegion *pRegionCur = GetNext();
		while (pRegionCur)
			{
			CRegion *pRegionNext = pRegionCur->GetNext();
			if (pRegionCur->m_usRegionSaved == m_usRegionSaved)
				{
				// delete region from linked list
				pRegionPrev->SetNext(pRegionNext);
				pRegionCur->Remove(pRegionCur);
				delete pRegionCur;

				// find the multichannel wave, if any
				// (wave may be monochannel when it's assigned because of ID match,
				// which can happen after a copy/paste instrument without copying waves)
				pWave = pWave->GetNode()->GetWave();
				}
			else
				pRegionPrev = pRegionCur;
			pRegionCur = pRegionNext;
			}
		}
}

