// ArticulationList.cpp : implementation file
//

#include "stdafx.h"
#include "DlsDefsPlus.h"
#include "DLSDesignerDLL.h"
#include "DLSDesigner.h"

#include "Collection.h"
#include "Instrument.h"
#include "Region.h"
#include "ConditionalChunk.h"
#include "Articulation.h"
#include "ArticulationList.h"
#include "InstrumentCtl.h"
#include "InstrumentFVEditor.h"

#include "ArticulationListPropPageMgr.h"
#include "ArticulationListPropPage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArticulationList constructor/destructor

CArticulationList::CArticulationList(CDLSComponent* pComponent, CInstrument* pOwnerInstrument) :
m_dwRef(0),
m_pIRootNode(NULL),
m_pIParentNode(NULL),
m_pArticListPropPageMgr(NULL),
m_pDLSComponent(pComponent),
m_pInstrument(pOwnerInstrument),
m_pOwnerRegion(NULL),
m_pConditionalChunk(NULL),
m_bDLS1(FALSE),
m_bConditionOK(TRUE),
m_bOwnerIsInstrument(TRUE)
{
	AddRef();
	CArticulation* pArticulation = new CArticulation(pComponent, pOwnerInstrument);
	AddTail(pArticulation);

	if(m_pArticListPropPageMgr == NULL)
	{
		m_pArticListPropPageMgr = new CArticulationListPropPageMgr();
	}

	if(m_pConditionalChunk == NULL)
	{
		m_pConditionalChunk = new CConditionalChunk(m_pDLSComponent);
	}
}


CArticulationList::CArticulationList(CDLSComponent* pComponent, CRegion* pOwnerRegion) :
m_dwRef(0),
m_pIRootNode(NULL),
m_pIParentNode(NULL),
m_pArticListPropPageMgr(NULL),
m_pDLSComponent(pComponent),
m_pInstrument(NULL),
m_pOwnerRegion(pOwnerRegion),
m_pConditionalChunk(NULL),
m_bDLS1(FALSE),
m_bConditionOK(TRUE),
m_bOwnerIsInstrument(FALSE)
{
	AddRef();
	m_pInstrument = pOwnerRegion->GetInstrument();
	CArticulation* pArticulation = new CArticulation(pComponent, pOwnerRegion);
	AddTail(pArticulation);

	if(m_pArticListPropPageMgr == NULL)
	{
		m_pArticListPropPageMgr = new CArticulationListPropPageMgr();
	}
	
	if(m_pConditionalChunk == NULL)
	{
		m_pConditionalChunk = new CConditionalChunk(m_pDLSComponent);
	}
}

CArticulationList::~CArticulationList()
{

	// If we are in a Property Page Manager we need to remove ourselves before we go away
	IDMUSProdPropSheet *pIPropSheet;
	if(SUCCEEDED(m_pDLSComponent->m_pIFramework->QueryInterface(IID_IDMUSProdPropSheet, (void**) &pIPropSheet)))
	{
		if(pIPropSheet->IsEqualPageManagerObject(this) == S_OK)
		{
			pIPropSheet->RemovePageManagerByObject(this);
		}
		else
		{
			if(m_pArticListPropPageMgr)
			{
				m_pArticListPropPageMgr->RemoveObject(this);
			}
		}
		pIPropSheet->Release();
	}

	if(m_pArticListPropPageMgr)
	{
		DWORD dwRefCount = m_pArticListPropPageMgr->Release();
		if(dwRefCount == 0)
		{
			m_pArticListPropPageMgr = NULL;
		}
	}


	while(!IsEmpty())
	{
        CArticulation* pArticulation = RemoveHead();
        pArticulation->Release();
	}
	
	if(m_pConditionalChunk)
	{
		delete m_pConditionalChunk;
		m_pConditionalChunk = NULL;
	}

}

void CArticulationList::AddTail(CArticulation* pINode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pINode);
	
	AList::AddTail((AListItem*) pINode);
}

CArticulation* CArticulationList::GetHead() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return (CArticulation*) AList::GetHead();
}

CArticulation* CArticulationList::RemoveHead() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return (CArticulation*) AList::RemoveHead();
}

void CArticulationList::Remove(CArticulation* pINode) 
{ 
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pINode);
	
	AList::Remove((AListItem*) pINode);
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IUnknown implementation

HRESULT CArticulationList::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if(::IsEqualIID(riid, IID_IDMUSProdNode) || ::IsEqualIID(riid, IID_IUnknown))
    {
        AddRef();
        *ppvObj = this;
        return S_OK;
    }

	if(::IsEqualIID(riid, IID_IPersistStream))
	{
		AddRef();
		*ppvObj = (IPersistStream*)this;
        return S_OK;
    }

	if(::IsEqualIID(riid, IID_IDMUSProdPropPageObject))
    {
        AddRef();
        *ppvObj = (IDMUSProdPropPageObject *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CArticulationList::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CArticulationList::Release()
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
// CArticulationList IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdNode::GetNodeImageIndex

HRESULT CArticulationList::GetNodeImageIndex(short* pnFirstImage)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pnFirstImage);
	
	ASSERT(m_pDLSComponent != NULL);

	if(m_bConditionOK)
	{
		return(m_pDLSComponent->GetFolderImageIndex(pnFirstImage));
	}
	else
	{
		return(m_pDLSComponent->GetFolderGrayedImageIndex(pnFirstImage));
	}
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdNode::GetComponent

HRESULT CArticulationList::GetComponent(IDMUSProdComponent** ppIComponent)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(m_pDLSComponent != NULL);
	ASSERT(ppIComponent);

	return m_pDLSComponent->QueryInterface(IID_IDMUSProdComponent, (void**)ppIComponent);
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdNode::GetRootNode

HRESULT CArticulationList::GetDocRootNode(IDMUSProdNode** ppIRootNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pIRootNode != NULL);

	m_pIRootNode->AddRef();
	*ppIRootNode = m_pIRootNode;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdNode::SetRootNode

HRESULT CArticulationList::SetDocRootNode(IDMUSProdNode* pIRootNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIRootNode != NULL );

	m_pIRootNode = pIRootNode;
//	m_pIRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdNode::GetParentNode

HRESULT CArticulationList::GetParentNode(IDMUSProdNode** ppIParentNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pIParentNode != NULL);

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdNode::SetParentNode

HRESULT CArticulationList::SetParentNode(IDMUSProdNode* pIParentNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdNode::GetNodeId

HRESULT CArticulationList::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_ArticulationFolderNode;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdNode::GetNodeName

HRESULT CArticulationList::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pbstrName);

	CString strName;
	TCHAR achBuffer[BUFFER_128];

	if(::LoadString(theApp.m_hInstance, IDS_ARTICULATION_FOLDER_NAME, achBuffer, BUFFER_128))
	{
		strName = achBuffer;
	}

    *pbstrName = strName.AllocSysString();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdNode::GetNodeNameMaxLength

HRESULT CArticulationList::GetNodeNameMaxLength(short* pnMaxLength)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pnMaxLength);
	
	*pnMaxLength = -1; // Can't rename a Articulation folder

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdNode::ValidateNodeName

HRESULT CArticulationList::ValidateNodeName(BSTR bstrName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Just free bstrName; can't rename Articulations folder
	::SysFreeString(bstrName);

	return S_OK;

}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdNode::SetNodeName

HRESULT CArticulationList::SetNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL; // Can't rename a Articulation folder
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdNode::GetEditorClsId

HRESULT CArticulationList::GetEditorClsId( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL; // Can't edit a Articulation folder
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdNode::GetEditorTitle

HRESULT CArticulationList::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL; // Can't edit a Articulation folder
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdNode::GetEditorWindow

HRESULT CArticulationList::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL; // Can't edit a Articulation folder
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdNode::SetEditorWindow

HRESULT CArticulationList::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL; // Can't edit a Articulation folder
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdNode::UseOpenCloseImages

HRESULT CArticulationList::UseOpenCloseImages(BOOL* pfUseOpenCloseImages)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pfUseOpenCloseImages);

	*pfUseOpenCloseImages = TRUE;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdNode::GetRightClickMenuId

HRESULT CArticulationList::GetRightClickMenuId(HINSTANCE* phInstance, UINT* pnMenuId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pnMenuId);
	ASSERT(phInstance);

	*phInstance = theApp.m_hInstance;
	*pnMenuId = IDM_ARTICULATIONLIST_NODE_RMENU;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdNode::OnRightClickMenuInit

HRESULT CArticulationList::OnRightClickMenuInit(HMENU hMenu)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	BOOL status = EnableMenuItem(hMenu, IDM_NEW_ARTICULATION, MF_BYCOMMAND | MF_ENABLED);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdNode::OnRightClickMenuSelect

HRESULT CArticulationList::OnRightClickMenuSelect(long lCommandId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = E_FAIL;

	switch(lCommandId)
	{
		case IDM_NEW_ARTICULATION:
		{
			if(m_pInstrument)
			{
				if(FAILED(m_pInstrument->SaveStateForUndo(IDS_UNDO_INSERT_ARTICULATION)))
					return E_FAIL;
			}
			hr = InsertChildNode(NULL);
			break;
		}
		
		case IDM_PROPERTIES:
		{
			hr = OnShowProperties();
			break;
		}
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdNode::DeleteChildNode

HRESULT CArticulationList::DeleteChildNode(IDMUSProdNode* pIChildNode, BOOL fPromptUser)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pIChildNode != NULL);
	ASSERT(m_pDLSComponent != NULL);
	ASSERT(m_pDLSComponent->m_pIFramework != NULL);

	// Remove node from Project Tree
	if( m_pDLSComponent->m_pIFramework->RemoveNode(pIChildNode, fPromptUser) == S_FALSE )
	{
		return E_FAIL;
	}

	if(m_pInstrument)
	{
		if(FAILED(m_pInstrument->SaveStateForUndo(IDS_UNDO_ARTICULATION_DELETE)))
		{
			// Out Of Memory!!! Do we need to add the node back to the tre???
			// Will it go through??
			return E_FAIL;
		}
	}

	// Remove from list
	Remove((CArticulation*)pIChildNode);
	pIChildNode->Release();

	// Set flag so we know to save file 
	if(m_pInstrument)
	{
		if(GetCount() == 0)
		{
			if(m_bOwnerIsInstrument == false)
			{
				ASSERT(m_pOwnerRegion);
				m_pOwnerRegion->SetCurrentArticulation(NULL);
			}
			else
			{
				m_pInstrument->SetFirstGlobaArtAsCurrent();
				m_pInstrument->CheckRegionsForLocalArts();
			}
		}

		m_pInstrument->m_pCollection->SetDirtyFlag();
		m_pInstrument->UpdateInstrument();
	}
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdNode::InsertChildNode

HRESULT CArticulationList::InsertChildNode(IDMUSProdNode* pIChildNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pDLSComponent);
	if(m_pDLSComponent == NULL)
	{
		return E_FAIL;
	}
	if(m_pInstrument == NULL)
	{
		return E_FAIL;
	}
	
	if(pIChildNode== NULL)
	{
		if(m_bOwnerIsInstrument)
		{
			pIChildNode = new CArticulation(m_pDLSComponent, m_pInstrument);
		}
		else
		{
			ASSERT(m_pOwnerRegion);
			pIChildNode = new CArticulation(m_pDLSComponent, m_pOwnerRegion);
		}

		if(pIChildNode == NULL)
		{
			return E_OUTOFMEMORY;
		}
	}

	// Add to the articulation list
	AddTail((CArticulation*)pIChildNode);
	
	// Set root and parent node of ALL children
	theApp.SetNodePointers( pIChildNode, (IDMUSProdNode *)(m_pInstrument->m_pCollection), (IDMUSProdNode *)this );
	
	// Add node to Project Tree
	if(!SUCCEEDED(m_pDLSComponent->m_pIFramework->AddNode(pIChildNode, (IDMUSProdNode *)this)))
	{
		DeleteChildNode( pIChildNode, FALSE );
		return E_FAIL;
	}

	return S_OK;
} 

////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdNode::DeleteNode

HRESULT CArticulationList::DeleteNode( BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	if(m_pIParentNode)
	{
		HRESULT hr = m_pIParentNode->DeleteChildNode(this, fPromptUser);
		return hr;
	}

	return E_FAIL;
}

//////////////////////////////////////////////////////////////////////
//
//	CArticulationList IDMUSProdNode::GetFirstChild
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulationList::GetFirstChild(IDMUSProdNode** ppIFirstChildNode)
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
//	CArticulationList IDMUSProdNode::GetNextChild
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulationList::GetNextChild(IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode)
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

	IDMUSProdNode* pINode = (IDMUSProdNode*) ((CArticulation*) pIChildNode)->GetNext();

	if(pINode)
	{
		pINode->AddRef();
		*ppINextChildNode = pINode;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CArticulationList IDMUSProdNode::GetNodeListInfo
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulationList::GetNodeListInfo(DMUSProdListInfo* pListInfo)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pListInfo);
	
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdNode::OnNodeSelChanged

HRESULT CArticulationList::OnNodeSelChanged(BOOL fSelected)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
//
// CArticulationList IDMUSProdNode::CreateDataObject
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulationList::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CArticulationList IDMUSProdNode::CanCut
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulationList::CanCut()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;

}

/////////////////////////////////////////////////////////////////////////////
//
// CArticulationList IDMUSProdNode::CanCopy
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulationList::CanCopy()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;

}

/////////////////////////////////////////////////////////////////////////////
//
// CArticulationList IDMUSProdNode::CanDelete
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulationList::CanDelete()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;

}

/////////////////////////////////////////////////////////////////////////////
//
// CArticulationList IDMUSProdNode::CanDeleteChildNode
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulationList::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;

}

/////////////////////////////////////////////////////////////////////////////
//
// CArticulationList IDMUSProdNode::CanPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulationList::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;

}

/////////////////////////////////////////////////////////////////////////////
//
// CArticulationList IDMUSProdNode::PasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulationList::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;

}

/////////////////////////////////////////////////////////////////////////////
//
// CArticulationList IDMUSProdNode::CanChildPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulationList::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
												   BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;

}

/////////////////////////////////////////////////////////////////////////////
//
// CArticulationList IDMUSProdNode::ChildPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulationList::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;

}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdNode::GetObject

HRESULT CArticulationList::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CCollection IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdPropPageObject::GetData

HRESULT CArticulationList::GetData(void** ppData)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(ppData);

	*ppData = this;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdPropPageObject::SetData

HRESULT CArticulationList::SetData(void* pData)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pData);

	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdPropPageObject::OnRemoveFromPageManager

HRESULT CArticulationList::OnRemoveFromPageManager(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IDMUSProdPropPageObject::OnShowProperties

HRESULT CArticulationList::OnShowProperties(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = E_FAIL;

	ASSERT(m_pDLSComponent);
	ASSERT(m_pDLSComponent->m_pIFramework);

	// Show the Collection properties
	IDMUSProdPropSheet* pIPropSheet;

	hr = m_pDLSComponent->m_pIFramework->QueryInterface(IID_IDMUSProdPropSheet, (void**)&pIPropSheet);
	if(SUCCEEDED(hr))
	{
		hr = pIPropSheet->SetPageManager(m_pArticListPropPageMgr);
		
		if(SUCCEEDED(hr))
		{
			m_pArticListPropPageMgr->SetObject(this);
			pIPropSheet->SetActivePage(0); 
		}

		pIPropSheet->Show(TRUE);
		pIPropSheet->Release();
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CArticulationList IPersist::GetClassID

HRESULT CArticulationList::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( pClsId != NULL );

    memset( pClsId, 0, sizeof( CLSID ) );

    return E_NOTIMPL;
}



/////////////////////////////////////////////////////////////////////////////
// CArticulationList IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
//
// CArticulationList IPersistStream::IsDirty
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulationList::IsDirty()
{
    // I want to know if I am called
	ASSERT(FALSE);

	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CArticulationList IPersistStream::GetSizeMax
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulationList::GetSizeMax( ULARGE_INTEGER FAR* /*pcbSize*/ )
{
    // I want to know if I am called
	ASSERT(FALSE);

	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CArticulationList IPersistStream::Load
//
//////////////////////////////////////////////////////////////////////
HRESULT CArticulationList::Load(IStream* pIStream)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_POINTER;
	}

	DWORD cbRead = 0;

	// Read the number of articulations saved
	int nArtCount = 0;
	HRESULT hr = pIStream->Read((void*)&nArtCount, sizeof(int), &cbRead);
	if(FAILED(hr) || cbRead != sizeof(int))
	{
		return E_FAIL;
	}

	// Match up the number of saved articulations to those in hand
	int nExistentArts = GetCount();
	if(nExistentArts < nArtCount)
	{
		for(int nCount = 0; nCount < (nArtCount - nExistentArts); nCount++)
		{
			if(FAILED(hr = InsertChildNode(NULL)))
			{
				return hr;
			}
		}
	}
	else if(nExistentArts > nArtCount)
	{
		for(int nCount = 0; nCount < (nExistentArts - nArtCount); nCount++)
		{
			CArticulation* pArt = RemoveHead();
			ASSERT(pArt);
			if(pArt)
			{
				// Remove node from Project Tree
				pArt->SetDeleteFlag(true);
				if(m_pDLSComponent->m_pIFramework->RemoveNode(pArt, FALSE) == S_FALSE )
				{
					return E_FAIL;
				}
				pArt->Release();
			}
		}
	}

	// Now load into the articulations
	CArticulation* pArt = GetHead();
	while(pArt)
	{
		if(FAILED(hr = pArt->Load(pIStream)))
		{
			return hr;
		}

		pArt = pArt->GetNext();
	}

	return S_OK;
}

HRESULT CArticulationList::Save(IStream* pIStream, BOOL fCLearDirty)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	// Save the number of artculations in the list
	int nArtCount = GetCount();

	DWORD cbCount = 0;
	HRESULT hr = pIStream->Write((void*)&nArtCount, sizeof(int), &cbCount);
	if(FAILED(hr) || cbCount != sizeof(int)) 
	{
		return E_FAIL;
	}

	// Save the Articulations now
	CArticulation* pArt = GetHead();
	while(pArt)
	{
		if(FAILED(hr = pArt->Save(pIStream, FALSE)))
		{
			return hr;
		}

		pArt = pArt->GetNext();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CArticulationList::Load

HRESULT CArticulationList::Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain)
{    
	AFX_MANAGE_STATE(_afxModuleAddrThis);
		
	HRESULT hr = E_FAIL;
	
	MMCKINFO ck;
	ck.ckid = 0;
	ck.fccType = 0;

	// Delete all previous articulations
	DeleteAllArticulations();

	while(pIRiffStream->Descend(&ck, pckMain, MMIO_FINDCHUNK) == 0)
	{
		switch(ck.ckid)
		{
			case FOURCC_CDL:
			{
				ASSERT(m_pConditionalChunk);
				if(m_pConditionalChunk == NULL)
				{
					return E_FAIL;
				}
				
				if(FAILED(hr = m_pConditionalChunk->Load(pIRiffStream, &ck)))
				{
					return hr;
				}

				break;
			}

			case FOURCC_ART2:
			case FOURCC_ART1:
			{
				BOOL bDLS1 = (ck.ckid == FOURCC_ART1) ? TRUE : FALSE;

				CArticulation* pArticulation = NULL;
				if(m_bOwnerIsInstrument)
				{
					pArticulation = new CArticulation(m_pDLSComponent, m_pInstrument);
				}
				else
				{
					ASSERT(m_pOwnerRegion);
					pArticulation = new CArticulation(m_pDLSComponent, m_pOwnerRegion);
				}

				if(pArticulation == NULL)
				{
					return E_OUTOFMEMORY;
				}

				pArticulation->SetDLS1(bDLS1);

				if(FAILED(hr = pArticulation->Load(pIRiffStream)))
				{
					delete pArticulation;
					return E_FAIL;
				}

				// Add it to the list
				AddTail(pArticulation);

				break;
			}
			
			case mmioFOURCC('e','d','i','t'):   // Ignore edits while loading.
			{
                break;
			}

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

	return hr;
}



/////////////////////////////////////////////////////////////////////////////
// CArticulationList::Save

HRESULT CArticulationList::Save(IDMUSProdRIFFStream* pIRiffStream)
{
	// No articulations so don't do anything
	if(GetCount() == 0)
	{
		return S_OK;
	}

	HRESULT hr = E_FAIL;
	MMCKINFO ck;
	ck.fccType = FOURCC_LAR2;
	if(m_bDLS1 == TRUE)
	{
		ck.fccType = FOURCC_LART;
	}

	if(SUCCEEDED(hr = pIRiffStream->CreateChunk(&ck, MMIO_CREATELIST)))
	{
		// Save the conditional chunk first
		if(m_pConditionalChunk)
		{
			if(FAILED(m_pConditionalChunk->Save(pIRiffStream)))
			{
				return E_FAIL;
			}
		}

		// Now save all the articulations
		CArticulation* pArtic = GetHead();
		while(pArtic)
		{
			if(FAILED(hr = pArtic->Save(pIRiffStream)))
			{
				return hr;
			}

			pArtic = pArtic->GetNext();
		}

		pIRiffStream->Ascend(&ck, 0);
	}

	// Save the unknown chunks in the list
    m_UnknownChunk.Save(pIRiffStream);

	return hr;
}

void CArticulationList::SetOwner(CInstrument* pInstrument)
{
	ASSERT(pInstrument);
	if(pInstrument == NULL)
	{
		return;
	}

	m_pOwnerRegion = NULL;
	m_pInstrument = pInstrument;
	m_bOwnerIsInstrument = TRUE;

	CArticulation* pArtic = GetHead();
	while(pArtic)
	{
		pArtic->SetOwner(pInstrument);
		pArtic = pArtic->GetNext();
	}
}

void CArticulationList::SetOwner(CRegion* pRegion)
{
	ASSERT(pRegion);
	if(pRegion == NULL)
	{
		return;
	}

	m_pOwnerRegion = pRegion;
	m_pInstrument = pRegion->m_pInstrument;
	m_bOwnerIsInstrument = FALSE;

	CArticulation* pArtic = GetHead();
	while(pArtic)
	{
		pArtic->SetOwner(pRegion);
		pArtic = pArtic->GetNext();
	}
}


void CArticulationList::DeleteAllArticulations()
{
	while(!IsEmpty())
	{
		CArticulation* pArtic = GetHead();
		// This articulation might not be in the tree...
		// dummy instruments and first-time default articulations
		if(FAILED(pArtic->DeleteNode(FALSE)))
		{
			Remove(pArtic);
			delete pArtic;
		}
	}
}

void CArticulationList::SetDLS1(BOOL bDLS1)
{
	m_bDLS1 = bDLS1;
}

BOOL CArticulationList::IsDLS1()
{
	return m_bDLS1;
}

CConditionalChunk* CArticulationList::GetConditionalChunk()
{
	return m_pConditionalChunk;
}


CCollection* CArticulationList::GetCollection()
{
	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
	{
		return NULL;
	}

	return m_pInstrument->GetCollection();
}


HRESULT CArticulationList::SetCondition(const CString& sCondition)
{
	ASSERT(m_pConditionalChunk);
	if(m_pConditionalChunk == NULL)
	{
		return E_FAIL;
	}

	return m_pConditionalChunk->RegularToRPN(sCondition);
}


DWORD CArticulationList::Count()
{
	if(!m_bConditionOK)
	{
		return 0;
	}

	// Must have an instrument right?
	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
	{
		return 0;
	}

	BOOL bNewFormat = FALSE;
	if(m_pInstrument)
	{
		bNewFormat = m_pInstrument->SupportsNewFormat();
	}

	DWORD dwCount = m_UnknownChunk.GetCount();
	CArticulation* pArticulation = GetHead();
	while(pArticulation)
	{
		dwCount += pArticulation->Count();

		// If the new format for downoad is not supported then use only the first articulation
		if(!bNewFormat)
		{
			break;
		}
		pArticulation = pArticulation->GetNext();
	}

	return  dwCount;
}


DWORD CArticulationList::Size()
{
	if(!m_bConditionOK)
	{
		return 0;
	}

	// Must have an instrument right?
	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
	{
		return 0;
	}

	BOOL bNewFormat = FALSE;
	if(m_pInstrument)
	{
		bNewFormat = m_pInstrument->SupportsNewFormat();
	}


	// Go through all the articulations and find out the largest required size
	DWORD dwSize = 0;
	CArticulation* pArticulation = GetHead();
	while(pArticulation)
	{
		dwSize += pArticulation->Size();

		// If the new format for downoad is not supported then use only the first articulation
		if(!bNewFormat)
		{
			break;
		}
		pArticulation = pArticulation->GetNext();
	}

	// Calculate the space need for the extension chunks
	CUnknownChunkItem* pExtChk = m_UnknownChunk.GetHead();
	for(; pExtChk; pExtChk = (CUnknownChunkItem*)pExtChk->GetNext())
	{
		dwSize += CHUNK_ALIGN(pExtChk->Size());
	}


	return dwSize;
}


HRESULT CArticulationList::Write(void* pv, DWORD* pdwRelativeOffset, DWORD* pdwCurrentOffset, DWORD* pDMWOffsetTable, DWORD* pdwCurIndex)
{
	HRESULT hr = E_FAIL;

	// Must have an instrument right?
	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
	{
		return E_FAIL;
	}

	if(!m_bConditionOK)
	{
		return S_OK;
	}

    // No articulations to write
    if(GetCount() == 0)
    {
        return S_OK;
    }


	BOOL bNewFormat = FALSE;
	if(m_pInstrument)
	{
		bNewFormat = m_pInstrument->SupportsNewFormat();
	}

	// Write down all articulations in series...
	CArticulation* pArticulation = GetHead();
	while(pArticulation)
	{
		DWORD dwNextArtIndex = 0;
		CArticulation* pNextArt = pArticulation->GetNext();
		if(pNextArt && bNewFormat)
		{
			dwNextArtIndex = *pdwCurIndex + pArticulation->Count();
		}

		pDMWOffsetTable[(*pdwCurIndex)++] = *pdwCurrentOffset;
		// Store current position to calculate new dwRelativeCurOffset.
        DWORD dwOffsetStart = *pdwCurrentOffset; 


		if(FAILED(hr = pArticulation->Write((BYTE*)pv + *pdwRelativeOffset, pdwCurrentOffset, pDMWOffsetTable, pdwCurIndex, dwNextArtIndex)))
		{
			return E_FAIL;
		}

		if(!m_bOwnerIsInstrument)
		{
			*pdwRelativeOffset += (*pdwCurrentOffset - dwOffsetStart);
		}
		else
		{
			*pdwRelativeOffset = *pdwCurrentOffset;
		}

		pArticulation = pArticulation->GetNext();
	}

	// Write down the extension chunks...
	CUnknownChunkItem* pExtChk = m_UnknownChunk.GetHead();
	DWORD dwCountExtChk = m_UnknownChunk.GetCount();
	DWORD dwIndexNextExtChk = 0;
	
	for(; pExtChk && SUCCEEDED(hr); pExtChk = (CUnknownChunkItem*) pExtChk->GetNext())
	{
		if(dwCountExtChk == 1)
		{
			dwIndexNextExtChk = 0;
		}
		else
		{
			dwIndexNextExtChk = *pdwCurIndex + 1;
		}

		pDMWOffsetTable[*pdwCurIndex] = *pdwCurrentOffset;
		hr = pExtChk->Write(((BYTE *)pv + *pdwCurrentOffset), pdwCurrentOffset,	dwIndexNextExtChk);
		
		dwCountExtChk--;
		(*pdwCurIndex)++;
	}

	return hr;
}


void CArticulationList::OnConditionConfigChanged(CSystemConfiguration* pCurrentConfig, bool bRefreshNode)
{
	m_bConditionOK = TRUE;

	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
	{
		m_bConditionOK = FALSE;
	}


	ASSERT(m_pDLSComponent);
	if(m_pDLSComponent == NULL)
	{
		m_bConditionOK = FALSE;
	}

	ASSERT(m_pDLSComponent->m_pIFramework);
	if(m_pDLSComponent->m_pIFramework == NULL)
	{
		m_bConditionOK = FALSE;
	}
	
	IDMUSProdFramework* pIFramework = m_pDLSComponent->m_pIFramework;
	ASSERT(m_pConditionalChunk);
	if(m_pConditionalChunk == NULL)
	{
		m_bConditionOK = FALSE;
	}

	// Need to evaluate the condition based on the configuration...something like...
	m_bConditionOK = m_pConditionalChunk->Evaluate(pCurrentConfig);

	// If this list is owned by a region, then give it a chance to revert to the 
	// global articulation in case no local articulation is going to be downloaded
	if(m_bOwnerIsInstrument == false)
	{
		ASSERT(m_pOwnerRegion);
		m_pOwnerRegion->CheckArtsAndSetInstrumentArt();
	}

	if(bRefreshNode)
	{
		CArticulation* pArt = GetHead();
		while(pArt)
		{
			pIFramework->RefreshNode(pArt);
			pArt = pArt->GetNext();
		}

		pIFramework->RefreshNode(this);
	}
}

BOOL CArticulationList::IsConditionOK()
{
	return m_bConditionOK;
}


void CArticulationList::CheckConfigAndRefreshNode()
{
	ASSERT(m_pConditionalChunk);
	if(m_pConditionalChunk == NULL)
	{
		m_bConditionOK = FALSE;
		return;
	}

	CSystemConfiguration*  pCurrentConfig = m_pDLSComponent->GetCurrentConfig();
	ASSERT(pCurrentConfig);

	OnConditionConfigChanged(pCurrentConfig, true);
	
	// If this list is owned by a region then ask the region to update for the change
	if(m_bOwnerIsInstrument == false)
	{
		m_pOwnerRegion->CheckArtsAndSetInstrumentArt();
	}

	if(m_pInstrument)
	{
		m_pInstrument->UpdateInstrument();
	}
}

void CArticulationList::SetConditionOK(BOOL bConditionOK, bool bRefreshNode)
{
	m_bConditionOK = bConditionOK;
	ASSERT(m_pDLSComponent);
	if(bRefreshNode && m_pDLSComponent)
	{
		// Refresh yourself
		IDMUSProdFramework* pIFramework = m_pDLSComponent->m_pIFramework;
		if(pIFramework)
		{
			// Refresh all articulation nodes
			CArticulation* pArt = GetHead();
			while(pArt)
			{
				pIFramework->RefreshNode(pArt);
				pArt = pArt->GetNext();
			}

			pIFramework->RefreshNode(this);
		}
	}
}

UINT CArticulationList::GetArticulationsUsed()
{
	// The articulation list doesn't get downloaded if the conditio is not met...
	if(m_bConditionOK)
	{
		return GetCount();
	}

	return 0;
}

CInstrument* CArticulationList::GetInstrument()
{
	return m_pInstrument;
}