// Articulation.cpp : implementation file
//

#include "stdafx.h"
#include "DLSDesignerDLL.h"
#include "DLSDesigner.h"

#include "Region.h"
#include "Articulation.h"
#include "ArticulationList.h"
#include "Collection.h"
#include "Instrument.h"
#include "InstrumentFVEditor.h"
#include "InstrumentCtl.h"
#include <math.h>

#include "resource.h"
#include "DLSLoadSaveUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CArticulation constructor/destructor

CArticulation::CArticulation(CDLSComponent* pComponent, CInstrument* pInstrument) :
m_pInstrument(pInstrument),
m_pRegion(NULL),
m_dwRef(0),
m_pIRootNode(NULL),
m_pIParentNode(NULL),
m_bDeletingArticulation(false),
m_bOwnerIsInstrument(TRUE),
#ifndef DMP_XBOX
m_bDLS1(TRUE)
#else
m_bDLS1(FALSE)
#endif
{
	ASSERT( pComponent != NULL );

	AddRef();
	m_pComponent = pComponent;
	m_pComponent->AddRef();

	InitDefaultArticParams();
}

CArticulation::CArticulation(CDLSComponent* pComponent, CRegion* pRegion) :
m_pInstrument(NULL),
m_pRegion(pRegion),
m_dwRef(0),
m_pIRootNode(NULL),
m_pIParentNode(NULL),
m_bDeletingArticulation(false),
m_bOwnerIsInstrument(FALSE),
#ifndef DMP_XBOX
m_bDLS1(TRUE)
#else
m_bDLS1(FALSE)
#endif
{
	ASSERT( pComponent != NULL );

	AddRef();
	m_pComponent = pComponent;
	m_pComponent->AddRef();

	InitDefaultArticParams();
}


CArticulation::~CArticulation()
{
	if(m_pComponent)
	{
		m_pComponent->Release();
	}
}


void CArticulation::InitDefaultArticParams()
{
	ZeroMemory(&m_ArticParams, sizeof(m_ArticParams));

	// Modulator LFO params
    m_ArticParams.m_LFO.m_tcDelay = Mils2TimeCents(0);
    m_ArticParams.m_LFO.m_pcFrequency = FIVE_HERTZ;

	// Vibrato LFO params
	m_ArticParams.m_LFO.m_tcDelayVibrato = Mils2TimeCents(0);
    m_ArticParams.m_LFO.m_pcFrequencyVibrato = FIVE_HERTZ;

	m_ArticParams.m_VolEG.m_tcDelay = Mils2TimeCents(0);
    m_ArticParams.m_VolEG.m_tcAttack = Mils2TimeCents(0);
	m_ArticParams.m_VolEG.m_tcHold = Mils2TimeCents(0);
    m_ArticParams.m_VolEG.m_tcDecay = Mils2TimeCents(0);
    m_ArticParams.m_VolEG.m_ptSustain = 1000 << 16;
    m_ArticParams.m_VolEG.m_tcRelease = Mils2TimeCents(0);
	m_ArticParams.m_VolEG.m_tcShutdownTime = 0;
    m_ArticParams.m_VolEG.m_tcVel2Attack = 0;
    m_ArticParams.m_VolEG.m_tcKey2Decay = 0;
	m_ArticParams.m_VolEG.m_tcKey2Hold = 0;
	m_ArticParams.m_PitchEG.m_tcDelay = Mils2TimeCents(0);
    m_ArticParams.m_PitchEG.m_tcAttack = Mils2TimeCents(0);
	m_ArticParams.m_PitchEG.m_tcHold = Mils2TimeCents(0);
    m_ArticParams.m_PitchEG.m_tcDecay = Mils2TimeCents(0);
    m_ArticParams.m_PitchEG.m_ptSustain = 1000 << 16;
    m_ArticParams.m_PitchEG.m_tcRelease = Mils2TimeCents(0);
    m_ArticParams.m_PitchEG.m_tcVel2Attack = 0;
	m_ArticParams.m_PitchEG.m_tcKey2Hold = 0;
    m_ArticParams.m_PitchEG.m_tcKey2Decay = 0;
    m_ArticParams.m_Misc.m_ptDefaultPan = 0; // Mid

	// Filter parameters
	m_ArticParams.m_FilterParams.m_pcInitialFc = 0x7FFFFFFF;
}


CArticulation* CArticulation::GetNext()
{
	return (CArticulation*) AListItem::GetNext();
}

DWORD CArticulation::PrepareTags(DWORD dwTag)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_dwEditTag = dwTag;
	
	if(dwTag) dwTag++;
	
	return dwTag;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulation IUnknown implementation

HRESULT CArticulation::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CArticulation::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CArticulation::Release()
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
// CArticulation IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::GetNodeImageIndex

HRESULT CArticulation::GetNodeImageIndex(short* pnFirstImage)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pnFirstImage);
	ASSERT(m_pComponent != NULL);

	BOOL bConditionOK = FALSE;

	IDMUSProdNode* pIParentNode =  NULL;
	GetParentNode(&pIParentNode);
	ASSERT(pIParentNode);
	if(pIParentNode)
	{
		CArticulationList* pArtList = dynamic_cast<CArticulationList*>(pIParentNode);
		if(pArtList)
		{
			bConditionOK = pArtList->IsConditionOK();
		}

		pIParentNode->Release();
	}

	if(bConditionOK)
	{
		return m_pComponent->GetArticulationImageIndex(pnFirstImage);
	}
	else
	{
		return m_pComponent->GetArticulationGrayedImageIndex(pnFirstImage);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::GetComponent

HRESULT CArticulation::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( m_pComponent != NULL );

	return m_pComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::GetRootNode

HRESULT CArticulation::GetDocRootNode( IDMUSProdNode** ppIRootNode )
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
// CArticulation IDMUSProdNode::SetRootNode

HRESULT CArticulation::SetDocRootNode( IDMUSProdNode* pIRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( pIRootNode != NULL );

	m_pIRootNode = pIRootNode;
//	m_pIRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::GetParentNode

HRESULT CArticulation::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_pIParentNode)
	{
		m_pIParentNode->AddRef();
		*ppIParentNode = m_pIParentNode;
		return S_OK;
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::SetParentNode

HRESULT CArticulation::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::GetNodeId

HRESULT CArticulation::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_ArticulationNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::GetNodeName

HRESULT CArticulation::GetNodeName(BSTR* pbstrName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pbstrName);

	CString cs = "Articulation";
    *pbstrName = cs.AllocSysString();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::GetNodeNameMaxLength

HRESULT CArticulation::GetNodeNameMaxLength(short* pnMaxLength)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pnMaxLength);
	
	*pnMaxLength = -1;	// Can't edit Articulation name.

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::ValidateNodeName

HRESULT CArticulation::ValidateNodeName(BSTR bstrName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Just free bstrName; can't rename an Articulation
	::SysFreeString(bstrName);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::SetNodeName

HRESULT CArticulation::SetNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	::SysFreeString( bstrName );
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::GetEditorClsId

HRESULT CArticulation::GetEditorClsId(CLSID* pClsId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pClsId);

	if(m_pInstrument)
	{
		return(m_pInstrument->GetEditorClsId(pClsId));
	}
	else
	{
		ASSERT(m_pRegion);
		ASSERT(m_pRegion->m_pInstrument);
		return(m_pRegion->m_pInstrument->GetEditorClsId(pClsId));
	}
}

/////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::GetEditorTitle

HRESULT CArticulation::GetEditorTitle(BSTR* pbstrTitle)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_pInstrument)
	{
		return(m_pInstrument->GetEditorTitle(pbstrTitle));
	}
	else
	{
		ASSERT(m_pRegion);
		ASSERT(m_pRegion->m_pInstrument);
		return(m_pRegion->m_pInstrument->GetEditorTitle(pbstrTitle));
	}
}

/////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::GetEditorWindow

HRESULT CArticulation::GetEditorWindow(HWND* hWndEditor)
{
	HRESULT hr = S_OK;
	*hWndEditor = NULL;

	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_pInstrument)
	{
		if ( m_bDeletingArticulation )
			*hWndEditor = NULL;
		else
			hr = m_pInstrument->GetEditorWindow(hWndEditor);
	}
	else
	{
		if ( (m_pRegion && m_pRegion->m_bDeletingRegion) || m_bDeletingArticulation )
			*hWndEditor = NULL;
		else if(m_pRegion && m_pRegion->m_pInstrument)
		{
			hr = m_pRegion->m_pInstrument->GetEditorWindow(hWndEditor);
		}
	}
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::SetEditorWindow

HRESULT CArticulation::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_pInstrument)
	{
		return(m_pInstrument->SetEditorWindow(hWndEditor));
	}
	else
	{
		ASSERT(m_pRegion);
		if(m_pRegion == NULL)
			return E_FAIL;

		ASSERT(m_pRegion->m_pInstrument);
		if(m_pRegion->m_pInstrument == NULL)
			return E_FAIL;

		return(m_pRegion->m_pInstrument->SetEditorWindow(hWndEditor));
	}
}

/////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::UseOpenCloseImages

HRESULT CArticulation::UseOpenCloseImages(BOOL* pfUseOpenCloseImages)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pfUseOpenCloseImages);

	*pfUseOpenCloseImages = TRUE;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::GetRightClickMenuId

HRESULT CArticulation::GetRightClickMenuId(HINSTANCE* phInstance, UINT* pnMenuId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pnMenuId);
	ASSERT(phInstance);

	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_ARTICULATION_NODE_RMENU;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::OnRightClickMenuInit

HRESULT CArticulation::OnRightClickMenuInit(HMENU hMenu)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	// if all regions do not have local art. gray the delete menu.
	if(CanDelete() != S_OK)
	{
		CMenu menu;
 		if( menu.Attach(hMenu) )
		{
			menu.EnableMenuItem( IDM_DELETE, (MF_GRAYED | MF_BYCOMMAND) );
			menu.Detach();
		}
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::OnRightClickMenuSelect

HRESULT CArticulation::OnRightClickMenuSelect(long lCommandId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pComponent != NULL);
	ASSERT(m_pComponent->m_pIFramework != NULL);

	HRESULT hr = E_FAIL;
	
	switch( lCommandId )
	{
		case IDM_OPEN:
		{
			if(SUCCEEDED(m_pComponent->m_pIFramework->OpenEditor(this)))
			{
				hr = S_OK;
				if(m_pInstrument)
				{
					m_pInstrument->SetCurrentArticulation(this);
				}

				else if(m_pRegion)
				{
					ASSERT(m_pRegion->m_pInstrument);
					if(m_pRegion->m_pInstrument)
						return E_FAIL;
					
					m_pRegion->SetCurrentArticulation(this);
				}
				else
				{
					// No owner for this articulation??
					ASSERT(1);
				}
			}
			
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
		{
			m_bDeletingArticulation = true;
			hr = DeleteNode(TRUE);
			break;
		}
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::DeleteChildNode

HRESULT CArticulation::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_FAIL;	// Not implemented.
}


/////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::InsertChildNode

HRESULT CArticulation::InsertChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_FAIL;	// Not implemented.
}


/////////////////////////////////////////////////////////////////////////////
// CArticulation IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CArticulation IPersist::GetClassID

HRESULT CArticulation::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
  
	ASSERT( pClsId != NULL );

    memset( pClsId, 0, sizeof( CLSID ) );

    return E_NOTIMPL;
}

////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::DeleteNode

HRESULT CArticulation::DeleteNode( BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	IDMUSProdNode* pIParentNode;

	if(SUCCEEDED(GetParentNode(&pIParentNode)))
	{
		CInstrument* pInstrument = NULL;
		if(m_pInstrument)
		{
			pInstrument = m_pInstrument;
		}
		else if(m_pRegion && m_pRegion->m_pInstrument)
		{
			pInstrument = m_pRegion->m_pInstrument;
		}

		if(pInstrument)
		{
			if(FAILED(pInstrument->SaveStateForUndo(IDS_UNDO_DELETE_ARTICULATION)))
			{
				return E_FAIL;
			}
		}
		
		m_bDeletingArticulation = true;
 		HRESULT hr = pIParentNode->DeleteChildNode((IDMUSProdNode *)this, fPromptUser);
		pIParentNode->Release();

		if(pInstrument)
		{
			pInstrument->RefreshRegion();
		}

		return hr;
	}
	
	return E_FAIL;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::GetFirstChild

HRESULT CArticulation::GetFirstChild(IDMUSProdNode** ppIFirstChildNode)
{
	// An Articulation does not have any children
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*ppIFirstChildNode = NULL;
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::GetNextChild

HRESULT CArticulation::GetNextChild(IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode)
{
	// An Articulation does not have any children
	
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::GetNodeListInfo

HRESULT CArticulation::GetNodeListInfo(DMUSProdListInfo* pListInfo)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pListInfo);
	
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::OnNodeSelChanged

HRESULT CArticulation::OnNodeSelChanged( BOOL fSelected )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CInstrument* pInstrument;

	if(m_pInstrument)
	{
		pInstrument = m_pInstrument;
	}
	else
	{
		pInstrument = m_pRegion->m_pInstrument;	
	}

	if(fSelected)
	{
		#ifdef _DEBUG
			ValidateParent();
		#endif

		
		if(m_pInstrument)
		{
			m_pInstrument->SetCurrentArticulation(this);
		}
		else if(m_pRegion)
		{
			m_pRegion->SetCurrentArticulation(this);
		}
		
		if(pInstrument)
		{
			pInstrument->m_pComponent->m_pIConductor->SetActiveTransport(pInstrument, BS_NO_AUTO_UPDATE | BS_PLAY_ENABLED);
		}

	}	
	else
	{
		pInstrument->m_pComponent->m_pIConductor->TransportStopped(pInstrument);
		pInstrument->TurnOffMidiNotes();
		pInstrument->m_pComponent->m_pIConductor->SetBtnStates(pInstrument,BS_NO_AUTO_UPDATE);
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
//
// CArticulation IDMUSProdNode::CreateDataObject
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulation::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CArticulation IDMUSProdNode::CanCut
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulation::CanCut()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;

}

/////////////////////////////////////////////////////////////////////////////
//
// CArticulation IDMUSProdNode::CanCopy
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulation::CanCopy()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;

}

/////////////////////////////////////////////////////////////////////////////
//
// CArticulation IDMUSProdNode::CanDelete
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulation::CanDelete()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    // If this is a global articulation, and
    // all regions in the instrument have articulations,
    // we should be able to delete the global articulation.
	// If its local, we should allow delete only if we 
	// have a global articulation

    bool bLocalArticulation = true;
	if(!m_pRegion && m_pInstrument) // if not an articulation of a region
	{
        // If we have multiple instrument articulations we should be able to delete this one
        if(m_pInstrument->GetArticulationCount() > 1)
		{
            return S_OK;
        }

		CRegion* pRegion = m_pInstrument->m_Regions.GetHead();
		ASSERT(pRegion);
        // walk thru region list to see if all regions have local articulations
        for(; pRegion && bLocalArticulation; pRegion = pRegion->GetNext())
        {
			if(pRegion->GetArticulationCount() == 0) 
			{
                bLocalArticulation = false;
				break;
            }
        }
	}
	else
	{
		CInstrument* pInstrument = m_pRegion->GetInstrument();
		ASSERT(pInstrument);
		if(pInstrument == NULL)
		{
			return S_FALSE;
		}

		if(m_pRegion->GetArticulationCount() > 1)
		{
			return S_OK;
		}
		else if(pInstrument->GetArticulationCount() == 0)
		{
			return S_FALSE;
		}
		
		return S_OK;
	}

	return bLocalArticulation ? S_OK : E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CArticulation IDMUSProdNode::CanDeleteChildNode
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulation::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;

}

/////////////////////////////////////////////////////////////////////////////
//
// CArticulation IDMUSProdNode::CanPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulation::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;

}

/////////////////////////////////////////////////////////////////////////////
//
// CArticulation IDMUSProdNode::PasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulation::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;

}

/////////////////////////////////////////////////////////////////////////////
//
// CArticulation IDMUSProdNode::CanChildPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulation::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
											  BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;

}

/////////////////////////////////////////////////////////////////////////////
//
// CArticulation IDMUSProdNode::ChildPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulation::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;

}


/////////////////////////////////////////////////////////////////////////////
// CArticulation IDMUSProdNode::GetObject

HRESULT CArticulation::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulation Additional functions

/////////////////////////////////////////////////////////////////////////////
// CArticulation::Save
HRESULT CArticulation::Save(IDMUSProdRIFFStream* pIRiffStream)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pIRiffStream);
	if(pIRiffStream == NULL)
	{
		return E_POINTER;
	}

	HRESULT hr = S_OK;			// Return code
	DWORD cb = 0;

	MMCKINFO ck;
    CONNECTIONLIST ConHeader;
	
	IStream* pIStream = NULL;
	pIStream = pIRiffStream->GetStream();
	if(pIStream == NULL)
	{
		return E_OUTOFMEMORY;
	}

	ck.ckid = FOURCC_ART2;
	if(m_bDLS1)
	{
		ck.ckid = FOURCC_ART1;
	}

    hr = pIRiffStream->CreateChunk(&ck, 0);
    
	if (FAILED(hr))
    {
		pIStream->Release();
        return (hr);
    }
	
	UINT nConnections = 0;
	CreateConnectionList(nConnections);

    ConHeader.cConnections = nConnections;
    ConHeader.cbSize = sizeof(ConHeader);
    hr = pIStream->Write((LPSTR) &ConHeader, sizeof(CONNECTIONLIST), &cb);	
    if (FAILED(hr) || cb != sizeof(CONNECTIONLIST))
    {
		pIStream->Release();
        return (hr);
    }
    hr = pIStream->Write((LPSTR) &m_arrConnections, sizeof(CONNECTION) * nConnections, &cb);	
    if (FAILED(hr))
    {
		pIStream->Release();
        return (hr);
    }

    if (FAILED(pIRiffStream->Ascend(&ck, 0)))
    {
		pIStream->Release();
        return (E_FAIL);
    }

	// Save unknown chunks 
    //m_UnknownChunk.Save(pIRiffStream);


	pIStream->Release();
    return (hr);
}

/////////////////////////////////////////////////////////////////////////////
// CArticulation::Load

HRESULT CArticulation::Load( IDMUSProdRIFFStream* pIRiffStream)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    IStream*		pIStream = NULL;
	DWORD			cb;
    CONNECTIONLIST	ConChunk;
    CONNECTION		Connection;
    DWORD			dwIndex;
    HRESULT			hr = S_OK;

	ASSERT( m_pComponent != NULL );

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    hr = pIStream->Read( &ConChunk, sizeof(ConChunk), &cb );
    if( FAILED(hr) )
	{
		pIStream->Release();
		return hr;
	}
	
    for (dwIndex = 0; dwIndex < ConChunk.cConnections; dwIndex++)
    {
		hr = pIStream->Read( &Connection, sizeof(CONNECTION), &cb );
		if( FAILED(hr))
		{
			pIStream->Release();
			return hr;
		}
        switch (Connection.usSource)
        {
			case CONN_SRC_NONE :
			{
				switch (Connection.usDestination)
				{
					// modulator LFO params
					case CONN_DST_LFO_FREQUENCY :
					{
						m_ArticParams.m_LFO.m_pcFrequency =	(PCENT) Connection.lScale;
						break;
					}
					case CONN_DST_LFO_STARTDELAY :
					{
						m_ArticParams.m_LFO.m_tcDelay =	(TCENT) Connection.lScale;
						break;
					}
					
					// Vibrato LFO params
					case CONN_DST_VIB_FREQUENCY :
					{
						m_ArticParams.m_LFO.m_pcFrequencyVibrato =	(PCENT) Connection.lScale;
						break;
					}
					case CONN_DST_VIB_STARTDELAY :
					{
						m_ArticParams.m_LFO.m_tcDelayVibrato =	(TCENT) Connection.lScale;
						break;
					}


					case CONN_DST_EG1_DELAYTIME :
					{
						m_ArticParams.m_VolEG.m_tcDelay = (TCENT) Connection.lScale;
						break;
					}

					case CONN_DST_EG1_ATTACKTIME :
					{
						m_ArticParams.m_VolEG.m_tcAttack = (TCENT) Connection.lScale;
						break;
					}
					
					case CONN_DST_EG1_HOLDTIME :
					{
						m_ArticParams.m_VolEG.m_tcHold = (TCENT) Connection.lScale;
						break;
					}

					case CONN_DST_EG1_DECAYTIME :
					{
						m_ArticParams.m_VolEG.m_tcDecay = (TCENT) Connection.lScale;
						break;
					}

					case CONN_DST_EG1_SUSTAINLEVEL :
					{
						m_ArticParams.m_VolEG.m_ptSustain = (PERCENT) Connection.lScale;
						break;
					}

					case CONN_DST_EG1_RELEASETIME :
					{
						m_ArticParams.m_VolEG.m_tcRelease = (TCENT) Connection.lScale; 
						break;
					}

					case CONN_DST_EG1_SHUTDOWNTIME :
					{
						m_ArticParams.m_VolEG.m_tcShutdownTime = (TCENT) Connection.lScale; 
						break;
					}

					case CONN_DST_EG2_DELAYTIME :
					{
						m_ArticParams.m_PitchEG.m_tcDelay = (TCENT) Connection.lScale;
						break;
					}

					case CONN_DST_EG2_ATTACKTIME :
					{
						m_ArticParams.m_PitchEG.m_tcAttack = (TCENT) Connection.lScale;
						break;
					}
					
					case CONN_DST_EG2_HOLDTIME :
					{
						m_ArticParams.m_PitchEG.m_tcHold = (TCENT) Connection.lScale;
						break;
					}

					case CONN_DST_EG2_DECAYTIME :
					{
						m_ArticParams.m_PitchEG.m_tcDecay = (TCENT) Connection.lScale;
						break;
					}

					case CONN_DST_EG2_SUSTAINLEVEL :
					{
						m_ArticParams.m_PitchEG.m_ptSustain = (PERCENT) Connection.lScale;
						break;
					}

					case CONN_DST_EG2_RELEASETIME :
					{
						m_ArticParams.m_PitchEG.m_tcRelease = (TCENT) Connection.lScale; 
						break;
					}

					case CONN_DST_PAN :
					{
						m_ArticParams.m_Misc.m_ptDefaultPan = (PERCENT) Connection.lScale;
						break;
					}

					case CONN_DST_FILTER_CUTOFF :
					{
						m_ArticParams.m_FilterParams.m_pcInitialFc = (PCENT) Connection.lScale;
						break;
					}

					case CONN_DST_FILTER_Q :
					{
						m_ArticParams.m_FilterParams.m_gcInitialQ = (GCENT) Connection.lScale;
						break;
					}
				}
				break;
			}
			case CONN_SRC_LFO :
			{
				switch (Connection.usControl)
				{
					case CONN_SRC_NONE :
					{
						switch (Connection.usDestination)
						{
							case CONN_DST_ATTENUATION :
							{
								m_ArticParams.m_LFO.m_gcVolumeScale = (GCENT) Connection.lScale; 
                                if(m_ArticParams.m_LFO.m_gcVolumeScale < 0)
                                {
                                    // Change the sign
                                    m_ArticParams.m_LFO.m_gcVolumeScale = (-m_ArticParams.m_LFO.m_gcVolumeScale);
                                }

								break;
							}

							case CONN_DST_PITCH :
							{
								m_ArticParams.m_LFO.m_pcPitchScale = (PCENT) Connection.lScale; 
								break;
							}

							case CONN_DST_FILTER_CUTOFF :
							{
								m_ArticParams.m_FilterParams.m_pcModLFOToFc = (PCENT) Connection.lScale;
								break;
							}
						}
						break;
					}
					case CONN_SRC_CC1 :
					{
						switch (Connection.usDestination)
						{
							case CONN_DST_ATTENUATION :
							{
								m_ArticParams.m_LFO.m_gcMWToVolume = (GCENT) Connection.lScale; 
								break;
							}

							case CONN_DST_PITCH :
							{
								m_ArticParams.m_LFO.m_pcMWToPitch = (PCENT) Connection.lScale; 
								break;
							}

							case CONN_DST_FILTER_CUTOFF :
							{
								m_ArticParams.m_FilterParams.m_pcModLFOCC1ToFc = (PCENT) Connection.lScale;
								break;
							}
						}
						break;
					}

					case CONN_SRC_CHANNELPRESSURE :
					{
						switch(Connection.usDestination)
						{
							case CONN_DST_GAIN :
							{
								m_ArticParams.m_LFO.m_gcChanPressToGain = (GCENT) Connection.lScale;
								break;
							}

							case CONN_DST_PITCH :
							{
								m_ArticParams.m_LFO.m_pcChanPressToPitch = (PCENT) Connection.lScale;
								break;
							}

							case CONN_DST_FILTER_CUTOFF :
							{
								m_ArticParams.m_LFO.m_pcChanPressToFc = (PCENT) Connection.lScale;
								break;
							}
						}
						
						break;
					}

				}
				break;
			}
			case CONN_SRC_VIBRATO :
			{
				switch(Connection.usControl)
				{
					case CONN_SRC_NONE :
					{
						switch(Connection.usDestination)
						{
							case CONN_DST_PITCH:
							{
								m_ArticParams.m_LFO.m_pcPitchScaleVibrato = (PCENT) Connection.lScale;
								break;
							}
						}
						break;
					}

					case CONN_SRC_CC1 :
					{
						switch(Connection.usDestination)
						{	
							case CONN_DST_PITCH :
							{
								m_ArticParams.m_LFO.m_pcMWToPitchVibrato = (PCENT) Connection.lScale;
								break;
							}

						}
						break;
					}
					case CONN_SRC_CHANNELPRESSURE :
					{
						switch(Connection.usDestination)
						{
							case CONN_DST_PITCH :
							{
								m_ArticParams.m_LFO.m_pcChanPressToPitchVibrato = (PCENT) Connection.lScale;
								break;
							}
						}
						break;
					}

				}
				break;
			}
			case CONN_SRC_KEYONVELOCITY :
			{
				switch (Connection.usDestination)
				{
					case CONN_DST_EG1_ATTACKTIME :
					{
						m_ArticParams.m_VolEG.m_tcVel2Attack = (TCENT) Connection.lScale;
						break;
					}
					case CONN_DST_EG2_ATTACKTIME :
					{
						m_ArticParams.m_PitchEG.m_tcVel2Attack = (TCENT) Connection.lScale;
						break;
					}
					case CONN_DST_ATTENUATION :
					{
						break;
					}
					case CONN_DST_FILTER_CUTOFF :
					{
						m_ArticParams.m_FilterParams.m_pcVelocityToFc = (PCENT) Connection.lScale;
						break;
					}
				}
				break;
			}
			case CONN_SRC_KEYNUMBER :
			{
				switch (Connection.usDestination)
				{
					case CONN_DST_EG1_DECAYTIME :
					{
						m_ArticParams.m_VolEG.m_tcKey2Decay = (TCENT) Connection.lScale;
						break;
					}

					case CONN_DST_EG2_DECAYTIME :
					{
						m_ArticParams.m_PitchEG.m_tcKey2Decay = (TCENT) Connection.lScale;
						break;
					}
					
					case CONN_DST_EG1_HOLDTIME :
					{
						m_ArticParams.m_VolEG.m_tcKey2Hold = (TCENT) Connection.lScale;
						break;
					}

					case CONN_DST_EG2_HOLDTIME :
					{
						m_ArticParams.m_PitchEG.m_tcKey2Hold = (TCENT) Connection.lScale;
						break;
					}

					case CONN_DST_FILTER_CUTOFF :
					{
						m_ArticParams.m_FilterParams.m_pcKeyNumToFc = (PCENT) Connection.lScale;
						break;
					}
				}
				break;
			}
			case CONN_SRC_EG2 :
			{
				switch (Connection.usDestination)
				{
					case CONN_DST_PITCH :
					{
						m_ArticParams.m_PitchEG.m_pcRange = (PCENT) Connection.lScale; 
						break;
					}

					case CONN_DST_FILTER_CUTOFF :
					{
						m_ArticParams.m_FilterParams.m_pcEGToFc = (PCENT) Connection.lScale;
						break;
					}
				}
				
				break;
			}
		}
    }

	pIStream->Release();
    return hr;
}


#ifdef _DEBUG
void CArticulation::ValidateParent()
{
	if(m_pInstrument == NULL && m_pRegion == NULL)
	{
		// Problem; we should never get here if we do 
		// Articulation does not have a parent; not good it 
		// needs one.
		ASSERT(FALSE);
	}
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CArticulation IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
//
// CArticulation IPersistStream::IsDirty
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulation::IsDirty()
{
    // I want to know if I am called
	ASSERT(FALSE);

	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CArticulation IPersistStream::GetSizeMax
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulation::GetSizeMax( ULARGE_INTEGER FAR* /*pcbSize*/ )
{
    // I want to know if I am called
	ASSERT(FALSE);

	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CArticulation IPersistStream::Load
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulation::Load(IStream* pIStream)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_POINTER;
	}

	ULONG ulBytesRead = 0;

	// DLS1 or DLS2?
	HRESULT hr = pIStream->Read((void *)&m_bDLS1, sizeof(BOOL), &ulBytesRead);
	if(FAILED(hr) || ulBytesRead != sizeof(BOOL))
	{
		return E_FAIL;
	}


	ulBytesRead = 0;
	hr = pIStream->Read((void *)&m_ArticParams, sizeof(ArticParams), &ulBytesRead);
	if(FAILED(hr) || ulBytesRead != sizeof(ArticParams))
	{
		return E_FAIL;
	}
	
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
//
// CArticulation IPersistStream::Save
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulation::Save(IStream* pIStream, BOOL fClearDirty)
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_POINTER;
	}

	ULONG ulBytesWritten = 0;
	
	// Save DLS1 or DLS2
	HRESULT hr = pIStream->Write((void *)&m_bDLS1, sizeof(BOOL), &ulBytesWritten);
	if(FAILED(hr) || ulBytesWritten != sizeof(BOOL))
	{
		return E_FAIL;
	}

	ulBytesWritten = 0;
	hr = pIStream->Write((void *)&m_ArticParams, sizeof(ArticParams), &ulBytesWritten);
	if(FAILED(hr) || ulBytesWritten != sizeof(ArticParams))
	{
		return E_FAIL;
	}

	return hr;
}

HRESULT CArticulation::Write(void* pv, DWORD* pdwCurOffset, DWORD* pDMWOffsetTable, DWORD* pdwCurIndex, DWORD dwNextArtIndex)
{
	ASSERT(pv);
	ASSERT(pdwCurOffset);
	ASSERT(pDMWOffsetTable);
	ASSERT(pdwCurIndex);
	
	BOOL bNewFormat = FALSE;
	if(m_pInstrument)
	{
		bNewFormat = m_pInstrument->SupportsNewFormat();
	}
	else if(m_pRegion)
	{
		ASSERT(m_pRegion->m_pInstrument);
		if(m_pRegion->m_pInstrument)
		{
			bNewFormat = m_pRegion->m_pInstrument->SupportsNewFormat();
		}
	}

	HRESULT hr = S_OK;
	DWORD dwRelativeCurOffset = 0;
	ULONG *pulFirstExtCkIdx = NULL;

	UINT nConnections = 0;
	CreateConnectionList(nConnections);
	DWORD dwConnectionListSize = (nConnections * sizeof CONNECTION) + sizeof CONNECTIONLIST;

	if(bNewFormat)
    {     
    	DMUS_ARTICULATION2* pArt = (DMUS_ARTICULATION2*) pv;
        dwRelativeCurOffset = CHUNK_ALIGN(sizeof(DMUS_ARTICULATION2));
	    
		(*pdwCurOffset) += CHUNK_ALIGN(sizeof(DMUS_ARTICULATION2));
        pArt->ulNextArtIdx = dwNextArtIndex;
        
		// Do we have any articulation data?
		if(dwConnectionListSize)
        {
            pDMWOffsetTable[*pdwCurIndex] = *pdwCurOffset;
		    pArt->ulArtIdx = (*pdwCurIndex)++;
            
			DWORD dwOffsetStart = *pdwCurOffset; 
			void* pvConnectionBuff = ((BYTE *)pv + dwRelativeCurOffset);

			CONNECTIONLIST* pList = (CONNECTIONLIST*) pvConnectionBuff;
			pList->cbSize = sizeof(CONNECTIONLIST);
			pList->cConnections = nConnections;

			CONNECTION* pConnection = (CONNECTION*) ((BYTE*) pvConnectionBuff + sizeof(CONNECTIONLIST));
			memcpy(pConnection, m_arrConnections, nConnections * sizeof(CONNECTION));
			
			*pdwCurOffset += dwConnectionListSize;
            dwRelativeCurOffset += (*pdwCurOffset - dwOffsetStart);	
        }
        else
	    {
		    pArt->ulArtIdx = 0;
	    }
    }
	else
	{

		DMUS_ARTICULATION* pArt = (DMUS_ARTICULATION *)pv;
		dwRelativeCurOffset = CHUNK_ALIGN(sizeof(DMUS_ARTICULATION));
		(*pdwCurOffset) += CHUNK_ALIGN(sizeof(DMUS_ARTICULATION));

		if(dwConnectionListSize)
		{
			pDMWOffsetTable[*pdwCurIndex] = *pdwCurOffset;
			pArt->ulArt1Idx = (*pdwCurIndex)++;
			
			DMUS_ARTICPARAMS articParamsDLS1;
			GetLevel1Articulation(articParamsDLS1);

			CopyMemory(((BYTE *)pv + dwRelativeCurOffset), &articParamsDLS1, CHUNK_ALIGN(sizeof(DMUS_ARTICPARAMS)));
			dwRelativeCurOffset += CHUNK_ALIGN(sizeof(DMUS_ARTICPARAMS));
			(*pdwCurOffset) += CHUNK_ALIGN(sizeof(DMUS_ARTICPARAMS));
		}
		else
		{
			pArt->ulArt1Idx = 0;
		}
	}

	return hr;
}

DWORD CArticulation::Size()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	DWORD dwSize = 0;
	BOOL bNewFormat = FALSE;
	if(m_pInstrument)
	{
		bNewFormat = m_pInstrument->SupportsNewFormat();
	}
	else if(m_pRegion)
	{
		ASSERT(m_pRegion->m_pInstrument);
		if(m_pRegion->m_pInstrument)
			bNewFormat = m_pRegion->m_pInstrument->SupportsNewFormat();
	}


	if(bNewFormat)
	{
		dwSize += CHUNK_ALIGN(sizeof(DMUS_ARTICULATION2));
	}
	else
	{
		dwSize += CHUNK_ALIGN(sizeof(DMUS_ARTICULATION));
	}

	UINT nConnections = 0;
	CreateConnectionList(nConnections);
	
	if(bNewFormat)
	{
		dwSize += nConnections * sizeof(CONNECTION) + sizeof(CONNECTIONLIST);
	}
	else
	{
		dwSize += CHUNK_ALIGN(sizeof(DMUS_ARTICPARAMS));
	}
	
	return dwSize;
}

DWORD CArticulation::Count()
{
	//return 1 + (m_bDLS1 ? TRUE : FALSE);
	return 2;
}

void CArticulation::CreateConnectionList(UINT& nConnections)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	nConnections = 0;

    // Connections for Modulator LFO
	if(notinrange(m_ArticParams.m_LFO.m_pcFrequency, FIVE_HERTZ, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_NONE,
					   CONN_SRC_NONE,
					   CONN_DST_LFO_FREQUENCY,
					   CONN_TRN_NONE,
					   (DWORD)m_ArticParams.m_LFO.m_pcFrequency);
    }

	if(notinrange(m_ArticParams.m_LFO.m_tcDelay, Mils2TimeCents(0), 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_NONE,
					   CONN_SRC_NONE,
					   CONN_DST_LFO_STARTDELAY,
					   CONN_TRN_NONE,
					   (DWORD)m_ArticParams.m_LFO.m_tcDelay);
    }

	if(notinrange(m_ArticParams.m_LFO.m_gcVolumeScale, 0, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_LFO,
					   CONN_SRC_NONE,
					   CONN_DST_ATTENUATION,
					   CONN_TRN_NONE,
					   (DWORD)m_ArticParams.m_LFO.m_gcVolumeScale);
    }
    
	if(notinrange(m_ArticParams.m_LFO.m_pcPitchScale,0,65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_LFO,
					   CONN_SRC_NONE,
					   CONN_DST_PITCH,
					   CONN_TRN_NONE,
					   (DWORD)m_ArticParams.m_LFO.m_pcPitchScale);
    }
    
	if(notinrange(m_ArticParams.m_LFO.m_gcMWToVolume, 0, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_LFO,
					   CONN_SRC_CC1,
					   CONN_DST_ATTENUATION,
					   CONN_TRN_NONE,
					   (DWORD)m_ArticParams.m_LFO.m_gcMWToVolume);
    }
    
	if(notinrange(m_ArticParams.m_LFO.m_pcMWToPitch, 0, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_LFO,
					   CONN_SRC_CC1,
					   CONN_DST_PITCH,
					   CONN_TRN_NONE,
					   (DWORD)m_ArticParams.m_LFO.m_pcMWToPitch);
    }

	if(notinrange(m_ArticParams.m_LFO.m_gcChanPressToGain, 0, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_LFO,
					   CONN_SRC_CHANNELPRESSURE,
					   CONN_DST_GAIN,
					   CONN_TRN_NONE,
					   (DWORD)m_ArticParams.m_LFO.m_gcChanPressToGain);
    }

	if(notinrange(m_ArticParams.m_LFO.m_pcChanPressToPitch, 0, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_LFO,
					   CONN_SRC_CHANNELPRESSURE,
					   CONN_DST_PITCH,
					   CONN_TRN_NONE,
					   (DWORD)m_ArticParams.m_LFO.m_pcChanPressToPitch);
    }

	if(notinrange(m_ArticParams.m_LFO.m_pcChanPressToFc, 0, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_LFO,
					   CONN_SRC_CHANNELPRESSURE,
					   CONN_DST_FILTER_CUTOFF,
					   CONN_TRN_NONE,
					   (DWORD)m_ArticParams.m_LFO.m_pcChanPressToFc);
    }

	// Connections for Vibrato LFO
	if(notinrange(m_ArticParams.m_LFO.m_pcFrequencyVibrato, FIVE_HERTZ, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_NONE,
					   CONN_SRC_NONE,
					   CONN_DST_VIB_FREQUENCY,
					   CONN_TRN_NONE,
					   (DWORD)m_ArticParams.m_LFO.m_pcFrequencyVibrato);
    }
    
	if(notinrange(m_ArticParams.m_LFO.m_tcDelayVibrato, Mils2TimeCents(0), 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_NONE,
					   CONN_SRC_NONE,
					   CONN_DST_VIB_STARTDELAY,
					   CONN_TRN_NONE,
					   (DWORD)m_ArticParams.m_LFO.m_tcDelayVibrato);
    }

	if(notinrange(m_ArticParams.m_LFO.m_pcPitchScaleVibrato,0,65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_VIBRATO,
					   CONN_SRC_NONE,
					   CONN_DST_PITCH,
					   CONN_TRN_NONE,
					   (DWORD)m_ArticParams.m_LFO.m_pcPitchScaleVibrato);
    }

	if(notinrange(m_ArticParams.m_LFO.m_pcMWToPitchVibrato, 0, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_VIBRATO,
					   CONN_SRC_CC1,
					   CONN_DST_PITCH,
					   CONN_TRN_NONE,
					   (DWORD)m_ArticParams.m_LFO.m_pcMWToPitchVibrato);
    }

	if(notinrange(m_ArticParams.m_LFO.m_pcChanPressToPitchVibrato, 0, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_VIBRATO,
					   CONN_SRC_CHANNELPRESSURE,
					   CONN_DST_PITCH,
					   CONN_TRN_NONE,
					   (DWORD)m_ArticParams.m_LFO.m_pcChanPressToPitchVibrato);
    }
	
	// Make connections for the Volume Envelope
	if(notinrange(m_ArticParams.m_VolEG.m_tcDelay, Mils2TimeCents(0), 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_NONE,
					   CONN_SRC_NONE,
					   CONN_DST_EG1_DELAYTIME,
					   CONN_TRN_NONE,
					   m_ArticParams.m_VolEG.m_tcDelay);
    }
    

	if(notinrange(m_ArticParams.m_VolEG.m_tcAttack, Mils2TimeCents(0), 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_NONE,
					   CONN_SRC_NONE,
					   CONN_DST_EG1_ATTACKTIME,
					   CONN_TRN_NONE,
					   m_ArticParams.m_VolEG.m_tcAttack);
    }

	if(notinrange(m_ArticParams.m_VolEG.m_tcHold, Mils2TimeCents(0), 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_NONE,
					   CONN_SRC_NONE,
					   CONN_DST_EG1_HOLDTIME,
					   CONN_TRN_NONE,
					   m_ArticParams.m_VolEG.m_tcHold);
    }

	if(notinrange(m_ArticParams.m_VolEG.m_tcDecay, Mils2TimeCents(0), 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_NONE,
					   CONN_SRC_NONE,
					   CONN_DST_EG1_DECAYTIME,
					   CONN_TRN_NONE,
					   m_ArticParams.m_VolEG.m_tcDecay);
    }
    
	if(notinrange(m_ArticParams.m_VolEG.m_ptSustain, 1000 << 16, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_NONE,
					   CONN_SRC_NONE,
					   CONN_DST_EG1_SUSTAINLEVEL,
					   CONN_TRN_NONE,
					   m_ArticParams.m_VolEG.m_ptSustain);
    }
    
	if(notinrange(m_ArticParams.m_VolEG.m_tcRelease, Mils2TimeCents(0), 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_NONE,
					   CONN_SRC_NONE,
					   CONN_DST_EG1_RELEASETIME,
					   CONN_TRN_NONE,
					   m_ArticParams.m_VolEG.m_tcRelease);
    }
    
	if(notinrange(m_ArticParams.m_VolEG.m_tcVel2Attack, 0, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_KEYONVELOCITY,
					   CONN_SRC_NONE,
					   CONN_DST_EG1_ATTACKTIME,
					   CONN_TRN_NONE,m_ArticParams.m_VolEG.m_tcVel2Attack);
    }
    
	if(notinrange(m_ArticParams.m_VolEG.m_tcKey2Decay, 0, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_KEYNUMBER,
					   CONN_SRC_NONE,
		               CONN_DST_EG1_DECAYTIME,
					   CONN_TRN_NONE,m_ArticParams.m_VolEG.m_tcKey2Decay);
    }

	if(notinrange(m_ArticParams.m_VolEG.m_tcKey2Hold, 0, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_KEYNUMBER,
					   CONN_SRC_NONE,
		               CONN_DST_EG1_HOLDTIME,
					   CONN_TRN_NONE,m_ArticParams.m_VolEG.m_tcKey2Hold);
    }
	
	if(notinrange(m_ArticParams.m_VolEG.m_tcShutdownTime, 0, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_NONE,
					   CONN_SRC_NONE,
		               CONN_DST_EG1_SHUTDOWNTIME,
					   CONN_TRN_NONE,m_ArticParams.m_VolEG.m_tcShutdownTime);
    }

	if(notinrange(m_ArticParams.m_PitchEG.m_tcDelay, Mils2TimeCents(0), 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_NONE,
					   CONN_SRC_NONE,
					   CONN_DST_EG2_DELAYTIME,
					   CONN_TRN_NONE,
					   m_ArticParams.m_PitchEG.m_tcDelay);
    }
    
	if(notinrange(m_ArticParams.m_PitchEG.m_tcAttack, Mils2TimeCents(0), 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_NONE,
					   CONN_SRC_NONE,
					   CONN_DST_EG2_ATTACKTIME,
					   CONN_TRN_NONE,
					   m_ArticParams.m_PitchEG.m_tcAttack);
    }

	if(notinrange(m_ArticParams.m_PitchEG.m_tcHold, Mils2TimeCents(0), 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_NONE,
					   CONN_SRC_NONE,
					   CONN_DST_EG2_HOLDTIME,
					   CONN_TRN_NONE,
					   m_ArticParams.m_PitchEG.m_tcHold);
    }
    
	if(notinrange(m_ArticParams.m_PitchEG.m_tcDecay, Mils2TimeCents(0), 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_NONE,
					   CONN_SRC_NONE,
					   CONN_DST_EG2_DECAYTIME,
					   CONN_TRN_NONE,
					   m_ArticParams.m_PitchEG.m_tcDecay);
    }
    
	if(notinrange(m_ArticParams.m_PitchEG.m_ptSustain, 1000 << 16, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_NONE,
					   CONN_SRC_NONE,
					   CONN_DST_EG2_SUSTAINLEVEL,
					   CONN_TRN_NONE,m_ArticParams.m_PitchEG.m_ptSustain);
    }
    
	if(notinrange(m_ArticParams.m_PitchEG.m_tcRelease, Mils2TimeCents(0), 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_NONE,
					   CONN_SRC_NONE,
					   CONN_DST_EG2_RELEASETIME,
					   CONN_TRN_NONE,
					   m_ArticParams.m_PitchEG.m_tcRelease);
    }
    if(notinrange(m_ArticParams.m_PitchEG.m_tcVel2Attack, 0, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_KEYONVELOCITY,
					   CONN_SRC_NONE,
					   CONN_DST_EG2_ATTACKTIME,
					   CONN_TRN_NONE,
					   m_ArticParams.m_PitchEG.m_tcVel2Attack);
    }
    
	if(notinrange(m_ArticParams.m_PitchEG.m_tcKey2Decay, 0, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_KEYNUMBER,
					   CONN_SRC_NONE,
					   CONN_DST_EG2_DECAYTIME,
					   CONN_TRN_NONE,
					   m_ArticParams.m_PitchEG.m_tcKey2Decay);
    }

	if(notinrange(m_ArticParams.m_PitchEG.m_tcKey2Hold, 0, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_KEYNUMBER,
					   CONN_SRC_NONE,
					   CONN_DST_EG2_HOLDTIME,
					   CONN_TRN_NONE,
					   m_ArticParams.m_PitchEG.m_tcKey2Hold);
    }

    if (notinrange(m_ArticParams.m_Misc.m_ptDefaultPan, 0, 65535))	
    {
		makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_NONE,
					   CONN_SRC_NONE,
					   CONN_DST_PAN,
					   CONN_TRN_NONE,
					   m_ArticParams.m_Misc.m_ptDefaultPan);
    }

    if (notinrange(m_ArticParams.m_PitchEG.m_pcRange, 0, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_EG2,
					   CONN_SRC_NONE,
					   CONN_DST_PITCH,
					   CONN_TRN_NONE,
					   m_ArticParams.m_PitchEG.m_pcRange);
    }

	// Filter parameters
	if (notinrange(m_ArticParams.m_FilterParams.m_pcInitialFc, 0, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_NONE,
					   CONN_SRC_NONE,
					   CONN_DST_FILTER_CUTOFF,
					   CONN_TRN_NONE,
					   m_ArticParams.m_FilterParams.m_pcInitialFc);
    }

	if (notinrange(m_ArticParams.m_FilterParams.m_gcInitialQ, 0, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_NONE,
					   CONN_SRC_NONE,
					   CONN_DST_FILTER_Q,
					   CONN_TRN_NONE,
					   m_ArticParams.m_FilterParams.m_gcInitialQ);
    }

	if (notinrange(m_ArticParams.m_FilterParams.m_pcModLFOToFc, 0, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_LFO,
					   CONN_SRC_NONE,
					   CONN_DST_FILTER_CUTOFF,
					   CONN_TRN_NONE,
					   m_ArticParams.m_FilterParams.m_pcModLFOToFc);
    }

	if (notinrange(m_ArticParams.m_FilterParams.m_pcModLFOCC1ToFc, 0, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_NONE,
					   CONN_SRC_CC1,
					   CONN_DST_FILTER_CUTOFF,
					   CONN_TRN_NONE,
					   m_ArticParams.m_FilterParams.m_pcModLFOCC1ToFc);
    }

	if (notinrange(m_ArticParams.m_FilterParams.m_pcEGToFc, 0, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_EG2,
					   CONN_SRC_NONE,
					   CONN_DST_FILTER_CUTOFF,
					   CONN_TRN_NONE,
					   m_ArticParams.m_FilterParams.m_pcEGToFc);
    }

	if (notinrange(m_ArticParams.m_FilterParams.m_pcVelocityToFc, 0, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_KEYONVELOCITY,
					   CONN_SRC_NONE,
					   CONN_DST_FILTER_CUTOFF,
					   CONN_TRN_NONE,
					   m_ArticParams.m_FilterParams.m_pcVelocityToFc);
    }

	if (notinrange(m_ArticParams.m_FilterParams.m_pcKeyNumToFc, 0, 65535))
    {
        makeconnection(&m_arrConnections[nConnections++],
					   CONN_SRC_KEYNUMBER,
					   CONN_SRC_NONE,
					   CONN_DST_FILTER_CUTOFF,
					   CONN_TRN_NONE,
					   m_ArticParams.m_FilterParams.m_pcKeyNumToFc);
    }

	ASSERT(nConnections <= 24);
}

void CArticulation::GetLevel1Articulation(DMUS_ARTICPARAMS& articParamsDLS1)
{
    ZeroMemory(&articParamsDLS1, sizeof(DMUS_ARTICPARAMS));

    articParamsDLS1.LFO.tcDelay = m_ArticParams.m_LFO.m_tcDelay;
    articParamsDLS1.LFO.pcFrequency = m_ArticParams.m_LFO.m_pcFrequency;
	articParamsDLS1.LFO.gcMWToVolume = m_ArticParams.m_LFO.m_gcMWToVolume;
	articParamsDLS1.LFO.gcVolumeScale = m_ArticParams.m_LFO.m_gcVolumeScale;
	articParamsDLS1.LFO.pcMWToPitch = m_ArticParams.m_LFO.m_pcMWToPitch;
	articParamsDLS1.LFO.pcPitchScale = m_ArticParams.m_LFO.m_pcPitchScale;
    
	articParamsDLS1.VolEG.tcAttack = m_ArticParams.m_VolEG.m_tcAttack;
    articParamsDLS1.VolEG.tcDecay = m_ArticParams.m_VolEG.m_tcDecay;
    articParamsDLS1.VolEG.ptSustain = m_ArticParams.m_VolEG.m_ptSustain;
    articParamsDLS1.VolEG.tcRelease = m_ArticParams.m_VolEG.m_tcRelease;
    articParamsDLS1.VolEG.tcVel2Attack = m_ArticParams.m_VolEG.m_tcVel2Attack;
    articParamsDLS1.VolEG.tcKey2Decay = m_ArticParams.m_VolEG.m_tcKey2Decay;
	
	articParamsDLS1.PitchEG.tcAttack = m_ArticParams.m_PitchEG.m_tcAttack;
    articParamsDLS1.PitchEG.tcDecay = m_ArticParams.m_PitchEG.m_tcDecay;
    articParamsDLS1.PitchEG.ptSustain = m_ArticParams.m_PitchEG.m_ptSustain;
    articParamsDLS1.PitchEG.tcRelease = m_ArticParams.m_PitchEG.m_tcRelease;
    articParamsDLS1.PitchEG.tcVel2Attack = m_ArticParams.m_PitchEG.m_tcVel2Attack;
    articParamsDLS1.PitchEG.tcKey2Decay = m_ArticParams.m_PitchEG.m_tcKey2Decay;
	articParamsDLS1.PitchEG.pcRange = m_ArticParams.m_PitchEG.m_pcRange;
    
	articParamsDLS1.Misc.ptDefaultPan = m_ArticParams.m_Misc.m_ptDefaultPan; 
}

void CArticulation::SetOwner(CInstrument* pInstrument)
{
	ASSERT(pInstrument);
	if(pInstrument)
	{
		m_pInstrument = pInstrument;
		m_pRegion = NULL;
	}
}

void CArticulation::SetOwner(CRegion* pRegion)
{
	ASSERT(pRegion);
	if(pRegion)
	{
		m_pRegion = pRegion;
		m_pInstrument = NULL;
	}
}

void* CArticulation::GetOwner(BOOL& bOwnerIsRegion)
{
	if(m_pInstrument)
	{
		bOwnerIsRegion = FALSE;
		return m_pInstrument;
	}
	else if(m_pRegion)
	{
		bOwnerIsRegion = TRUE;
		return m_pRegion;
	}

	// ERROR!!
	ASSERT(1);
	return NULL;
}

CInstrument* CArticulation::GetInstrument()
{
	if(m_pInstrument)
	{
		return m_pInstrument;
	}
	ASSERT(m_pRegion);
	if(m_pRegion)
	{
		return m_pRegion->GetInstrument();
	}

	return NULL;
}

void CArticulation::SetDLS1(BOOL bDLS1)
{
	m_bDLS1 = bDLS1;
    if(bDLS1)
    {
        m_ArticParams.m_FilterParams.m_pcInitialFc = 0x7FFFFFFF;
    }
}

BOOL CArticulation::IsDLS1()
{
	return m_bDLS1;
}

ArticParams* CArticulation::GetArticParams() 
{
	return(&m_ArticParams);
}


void CArticulation::SetDeleteFlag(bool bDeletingArt)
{
	m_bDeletingArticulation = bDeletingArt;
}


bool CArticulation::GetDeleteFlag()
{
	return m_bDeletingArticulation;
}


CRegion* CArticulation::GetRegion()
{
	return m_pRegion;
}