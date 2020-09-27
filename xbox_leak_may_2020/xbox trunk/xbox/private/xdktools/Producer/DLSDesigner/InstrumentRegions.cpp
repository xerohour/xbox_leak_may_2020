// InstrumentRegions.cpp : implementation file
//

#include "stdafx.h"
#include "DlsDefsPlus.h"
#include "DLSDesignerDLL.h"
#include "DLSDesigner.h"
#include "Collection.h"
#include "Instrument.h"
#include "Region.h"
#include "Articulation.h"
#include "InstrumentRegions.h"
#include "InstrumentCtl.h"
#include "InstrumentFVEditor.h"
#include "WaveNode.h"
#include "Wave.h"
#include "MonoWave.h"
#include "StereoWave.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions constructor/destructor

CInstrumentRegions::CInstrumentRegions() :
m_dwRef(0),
m_pIRootNode(NULL),
m_pIParentNode(NULL),
m_pDLSComponent(NULL),
m_pInstrument(NULL),
m_nNumberOfLayers(MINIMUM_LAYERS)
{
	AddRef();
}

CInstrumentRegions::~CInstrumentRegions()
{
	while(!IsEmpty())
	{
        CRegion *pRegion = RemoveHead();
        pRegion->Release();
	}
}

void CInstrumentRegions::AddTail(CRegion *pINode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pINode);
	
	AList::AddTail((AListItem *) pINode);
}

CRegion *CInstrumentRegions::GetHead() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return (CRegion *)AList::GetHead();
}

CRegion *CInstrumentRegions::RemoveHead() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return (CRegion *) AList::RemoveHead();
}

void CInstrumentRegions::Remove(CRegion *pINode) 
{ 
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pINode);
	
	AList::Remove((AListItem *) pINode);
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IUnknown implementation

HRESULT CInstrumentRegions::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdNode)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = this;
        return S_OK;
    }
	
	if(::IsEqualIID(riid, IID_IDMUSProdSortNode))
	{
		AddRef();
        *ppvObj = (IDMUSProdSortNode*) this;
        return S_OK;
	}

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CInstrumentRegions::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CInstrumentRegions::Release()
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
// CInstrumentRegions IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IDMUSProdNode::GetNodeImageIndex

HRESULT CInstrumentRegions::GetNodeImageIndex(short* pnFirstImage)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pnFirstImage);
	
	ASSERT(m_pDLSComponent != NULL);

	return(m_pDLSComponent->GetFolderImageIndex(pnFirstImage));
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IDMUSProdNode::GetComponent

HRESULT CInstrumentRegions::GetComponent(IDMUSProdComponent** ppIComponent)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(m_pDLSComponent != NULL);
	ASSERT(ppIComponent);

	return m_pDLSComponent->QueryInterface(IID_IDMUSProdComponent, (void**)ppIComponent);
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IDMUSProdNode::GetRootNode

HRESULT CInstrumentRegions::GetDocRootNode(IDMUSProdNode** ppIRootNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pIRootNode != NULL);

	m_pIRootNode->AddRef();
	*ppIRootNode = m_pIRootNode;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IDMUSProdNode::SetRootNode

HRESULT CInstrumentRegions::SetDocRootNode(IDMUSProdNode* pIRootNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIRootNode != NULL );

	m_pIRootNode = pIRootNode;
//	m_pIRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IDMUSProdNode::GetParentNode

HRESULT CInstrumentRegions::GetParentNode(IDMUSProdNode** ppIParentNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pIParentNode != NULL);

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IDMUSProdNode::SetParentNode

HRESULT CInstrumentRegions::SetParentNode(IDMUSProdNode* pIParentNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IDMUSProdNode::GetNodeId

HRESULT CInstrumentRegions::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_RegionFolderNode;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IDMUSProdNode::GetNodeName

HRESULT CInstrumentRegions::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pbstrName);

	CString strName;
	TCHAR achBuffer[BUFFER_128];

	if(::LoadString(theApp.m_hInstance, IDS_REGION_FOLDER_NAME, achBuffer, BUFFER_128))
	{
		strName = achBuffer;
	}

    *pbstrName = strName.AllocSysString();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IDMUSProdNode::GetNodeNameMaxLength

HRESULT CInstrumentRegions::GetNodeNameMaxLength(short* pnMaxLength)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pnMaxLength);
	
	*pnMaxLength = -1; // Can't rename a Region folder

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IDMUSProdNode::ValidateNodeName

HRESULT CInstrumentRegions::ValidateNodeName(BSTR bstrName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Just free bstrName; can't rename Regions folder
	::SysFreeString(bstrName);

	return S_OK;

}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IDMUSProdNode::SetNodeName

HRESULT CInstrumentRegions::SetNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL; // Can't rename a Region folder
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IDMUSProdNode::GetEditorClsId

HRESULT CInstrumentRegions::GetEditorClsId( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL; // Can't edit a Region folder
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IDMUSProdNode::GetEditorTitle

HRESULT CInstrumentRegions::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL; // Can't edit a Region folder
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IDMUSProdNode::GetEditorWindow

HRESULT CInstrumentRegions::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL; // Can't edit a Region folder
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IDMUSProdNode::SetEditorWindow

HRESULT CInstrumentRegions::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL; // Can't edit a Region folder
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IDMUSProdNode::UseOpenCloseImages

HRESULT CInstrumentRegions::UseOpenCloseImages(BOOL* pfUseOpenCloseImages)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pfUseOpenCloseImages);

	*pfUseOpenCloseImages = TRUE;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IDMUSProdNode::GetRightClickMenuId

HRESULT CInstrumentRegions::GetRightClickMenuId(HINSTANCE* phInstance, UINT* pnMenuId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pnMenuId);
	ASSERT(phInstance);

	*phInstance = theApp.m_hInstance;
	*pnMenuId = IDM_REGIONS_NODE_RMENU;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IDMUSProdNode::OnRightClickMenuInit

HRESULT CInstrumentRegions::OnRightClickMenuInit(HMENU hMenu)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	/*int nNextFreeNote = GetNextFreeNote();
	if(nNextFreeNote == -1)
	{
		BOOL status = EnableMenuItem(hMenu, IDM_NEW_REGION, MF_BYCOMMAND | MF_GRAYED);
	}
	else*/
	{
		BOOL status = EnableMenuItem(hMenu, IDM_NEW_REGION, MF_BYCOMMAND | MF_ENABLED);
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IDMUSProdNode::OnRightClickMenuSelect

HRESULT CInstrumentRegions::OnRightClickMenuSelect(long lCommandId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = E_FAIL;

	switch(lCommandId)
	{
		case IDM_NEW_REGION:
			if(m_pInstrument)
			{
				if(FAILED(m_pInstrument->SaveStateForUndo(IDS_UNDO_REGION_INSERT)))
					return E_FAIL;
			}
			hr = InsertChildNode(NULL);
			break;
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IDMUSProdNode::DeleteChildNode

HRESULT CInstrumentRegions::DeleteChildNode(IDMUSProdNode* pIChildNode, BOOL fPromptUser)
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

	if (m_pInstrument)
	{
		if(FAILED(m_pInstrument->SaveStateForUndo(IDS_UNDO_REGION_DELETE)))
		{
			// Out Of Memory!!! Do we need to add the node back to the tre???
			// Will it go through??
			return E_FAIL;
		}

		CInstrumentFVEditor* pIE = m_pInstrument->GetInstrumentEditor();
		CRegion* pRegion = (CRegion*) pIChildNode;
		if(pIE)
		{
			pIE->m_RegionKeyBoard.DeleteRegion(short(pRegion->GetLayer()), pRegion->m_rRgnHeader.RangeKey.usLow);
			pIE->SendRegionChange();
		}
	}

	// Remove from list
	Remove((CRegion *)pIChildNode);
	pIChildNode->Release();

	// Set flag so we know to save file 
	m_pInstrument->m_pCollection->SetDirtyFlag();

	m_pInstrument->UpdateInstrument();
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IDMUSProdNode::InsertChildNode

HRESULT CInstrumentRegions::InsertChildNode(IDMUSProdNode* pIChildNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	int nNextFreeNote = 0;
	USHORT usFreeLayer = 1; 
	GetNextFreeNote(usFreeLayer, nNextFreeNote);
	if(nNextFreeNote > -1)
		return(InsertRegion(pIChildNode, usFreeLayer, USHORT(nNextFreeNote), USHORT(nNextFreeNote)));
	else
		return E_FAIL;
}

/* creates a new region */
HRESULT CInstrumentRegions::InsertRegion(IDMUSProdNode* pIChildNode, int nLayer, USHORT lowerNote,
	USHORT upperNote, bool bInsertIntoKeyboard, CRegion **ppRegion)
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

	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
	{
		return E_UNEXPECTED;
	}
	
	if(pIChildNode == NULL)
	{
		pIChildNode = new CRegion(m_pDLSComponent);
		if(pIChildNode == NULL)
		{
			return E_OUTOFMEMORY;
		}
	}

	// assign a variable so we don't cast every time.
	CRegion * pRegion = (CRegion *)pIChildNode;
	if(bInsertIntoKeyboard)
	{
		if(m_pInstrument && m_pInstrument->GetInstrumentEditor() != NULL)
		{
			CInstrumentFVEditor* pIE = m_pInstrument->GetInstrumentEditor();
			if(pIE)
			{
				short nFirstNote = lowerNote;
				short nLastNote = upperNote;
				nLayer = pIE->m_RegionKeyBoard.InsertNewRegion(nFirstNote, nLastNote, 0, 127);
			}
		}
		else 
		{
			nLayer = 0;
		}
	}

	pRegion->SetLayer(nLayer);

	pRegion->m_pInstrument = m_pInstrument;
	pRegion->GetName();
	
	pRegion->m_rRgnHeader.RangeKey.usLow = lowerNote;
	pRegion->m_rRgnHeader.RangeKey.usHigh = upperNote;
	
	// Make sure we have an Articulation for the Region to use
	
	CInstrument* pInstrument = pRegion->GetInstrument();
	if(pInstrument && pInstrument->GetArticulationCount() == 0)
	{
		// Instrument does not have an Articulation so we need to create one
		pInstrument->CreateDefaultGlobalArticulation();
		if(pInstrument->m_pCurArticulation == NULL)
			goto LDeleteChildAndFail;
	}		

    // Setup a wave for the region.
	if(pRegion->m_pWave == NULL)
	{
		CWave* pWave = m_pInstrument->m_pCollection->m_Waves.GetFirstWave();
		if (!pWave)
			goto LDeleteChildAndFail;

		pRegion->SetWave(pWave);
	}

    // add to Region list
	AddTail( pRegion);

	// Set root and parent node of ALL children
	theApp.SetNodePointers( pIChildNode, (IDMUSProdNode *)(m_pInstrument->m_pCollection), (IDMUSProdNode *)this );

	// Add node to Project Tree
	if(!SUCCEEDED(pIFramework->AddNode(pIChildNode, (IDMUSProdNode *)this)))
	{
		DeleteChildNode( pIChildNode, FALSE );
		return E_FAIL;
	}

	if(m_pInstrument)
	{
		CInstrumentFVEditor* pIE;
		pIE = m_pInstrument->GetInstrumentEditor();
		if(pIE)
		{
			pIE->SetCurRegion(pRegion);
			pIE->SendRegionChange();
			pIE->m_RegionKeyBoard.SetWaveName(short(nLayer), lowerNote, pRegion->GetWaveName());
		}
	}

	bool fIsDrumKit;
	fIsDrumKit = (pRegion->m_pInstrument->m_rInstHeader.Locale.ulBank & F_INSTRUMENT_DRUMS) != 0;
	if(fIsDrumKit) // If a Drum Kit we want the Region to own the Articulation
	{
		//pRegion->m_pArticulation = new CArticulation(m_pDLSComponent, pRegion);
		pRegion->InsertChildNode(NULL);
		if(pRegion->m_pArticulation == NULL)
			goto LDeleteChildAndFail;

		pIFramework->SetSelectedNode(pRegion);
	}
	
	// Set flag to indicate the DLS collection now needs to be saved
	m_pInstrument->m_pCollection->SetDirtyFlag();

	if (ppRegion)
		*ppRegion = pRegion;

	return S_OK;

LDeleteChildAndFail:
	delete pIChildNode;
	return E_OUTOFMEMORY;
}

////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IDMUSProdNode::DeleteNode

HRESULT CInstrumentRegions::DeleteNode( BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////
//
//	CInstrumentRegions IDMUSProdNode::GetFirstChild
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrumentRegions::GetFirstChild(IDMUSProdNode** ppIFirstChildNode)
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
//	CInstrumentRegions IDMUSProdNode::GetNextChild
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrumentRegions::GetNextChild(IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode)
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

	IDMUSProdNode* pINode = (IDMUSProdNode*) ((CRegion *) pIChildNode)->GetNext();

	if(pINode)
	{
		pINode->AddRef();
		*ppINextChildNode = pINode;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CInstrumentRegions IDMUSProdNode::GetNodeListInfo
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrumentRegions::GetNodeListInfo(DMUSProdListInfo* pListInfo)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pListInfo);
	
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IDMUSProdNode::OnNodeSelChanged

HRESULT CInstrumentRegions::OnNodeSelChanged(BOOL fSelected)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
//
// CInstrumentRegions IDMUSProdNode::CreateDataObject
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrumentRegions::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CInstrumentRegions IDMUSProdNode::CanCut
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrumentRegions::CanCut()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;

}

/////////////////////////////////////////////////////////////////////////////
//
// CInstrumentRegions IDMUSProdNode::CanCopy
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrumentRegions::CanCopy()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;

}

/////////////////////////////////////////////////////////////////////////////
//
// CInstrumentRegions IDMUSProdNode::CanDelete
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrumentRegions::CanDelete()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;

}

/////////////////////////////////////////////////////////////////////////////
//
// CInstrumentRegions IDMUSProdNode::CanDeleteChildNode
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrumentRegions::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;

}

/////////////////////////////////////////////////////////////////////////////
//
// CInstrumentRegions IDMUSProdNode::CanPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrumentRegions::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;

}

/////////////////////////////////////////////////////////////////////////////
//
// CInstrumentRegions IDMUSProdNode::PasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrumentRegions::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;

}

/////////////////////////////////////////////////////////////////////////////
//
// CInstrumentRegions IDMUSProdNode::CanChildPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrumentRegions::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
												   BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;

}

/////////////////////////////////////////////////////////////////////////////
//
// CInstrumentRegions IDMUSProdNode::ChildPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrumentRegions::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;

}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IDMUSProdNode::GetObject

HRESULT CInstrumentRegions::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions IDMUSProdSortNode methods
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// IDMUSProdSortNode::CompareNodes

HRESULT CInstrumentRegions::CompareNodes(IDMUSProdNode* pNode1, IDMUSProdNode* pNode2, int* pnResult)
{
	ASSERT(pNode1);
	ASSERT(pNode2);
	
	if(pNode1 == NULL || pNode2 == NULL)
		return E_INVALIDARG;

	if(pnResult == NULL)
		return E_POINTER;

	CRegion* pRegion1 = dynamic_cast<CRegion*>(pNode1);
	CRegion* pRegion2 = dynamic_cast<CRegion*>(pNode2);
	
	if(pRegion1 == NULL || pRegion2 == NULL)
		return E_FAIL;

	if(pRegion1->m_rRgnHeader.RangeKey.usLow < pRegion2->m_rRgnHeader.RangeKey.usLow)
	{
		*pnResult = -1;
	}
	else if(pRegion1->m_rRgnHeader.RangeKey.usLow > pRegion2->m_rRgnHeader.RangeKey.usLow)
	{
		*pnResult = 1;
	}
	else 
	{
		*pnResult = 0;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CInstrumentRegions::Load

HRESULT CInstrumentRegions::Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain)
{    
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IStream* pIStream;
	MMCKINFO ck;
    HRESULT	hr = E_FAIL;

	ASSERT(m_pDLSComponent != NULL);

    pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream != NULL);

	ck.ckid = 0;
	ck.fccType = 0;
    while( pIRiffStream->Descend(&ck, pckMain, 0) == 0 )
	{
		switch (ck.ckid) 
		{
			case FOURCC_LIST :
				switch(ck.fccType)
				{
					case FOURCC_RGN :
					case FOURCC_RGN2:
					{
						bool bDLS1Region = (ck.fccType == FOURCC_RGN) ? true : false;
						CRegion *pRegion = new CRegion(m_pDLSComponent, bDLS1Region);
						if(pRegion != NULL)
						{
							pRegion->m_pInstrument = m_pInstrument;
							hr = pRegion->Load(pIRiffStream, &ck);
							if(SUCCEEDED(hr))
							{
								AddTail(pRegion);
							}
							else
							{
								pRegion->Release();
								pIStream->Release();
								return hr;
							}
						}
						else
						{
							pIStream->Release();
							return E_OUTOFMEMORY;
						}

						break;
					}
				}
			break;
		}
        pIRiffStream->Ascend( &ck, 0 );
		ck.ckid = 0;
		ck.fccType = 0;
	}

	pIStream->Release();
	return hr;
}



void CInstrumentRegions::GetNextFreeNote(USHORT& usLayer, int& nNote)
{

	USHORT usFirstLayer = 1;
	if(m_pInstrument && m_pInstrument->GetInstrumentEditor() != NULL)
	{
		usFirstLayer = m_pInstrument->GetInstrumentEditor()->GetActiveLayer();
	}

	for(usLayer = usFirstLayer; usLayer < m_nNumberOfLayers; usLayer++)
	{
		for(nNote = 0; nNote < 128; nNote++)
		{
			CRegion* pRegion = FindRegionFromMap(usLayer, short(nNote));
			if(pRegion == NULL)
			{
				return;
			}
			else
			{
				// Skip the search to the end of this region
				nNote = pRegion->m_rRgnHeader.RangeKey.usHigh;
			}

		}
	}


}

void CInstrumentRegions::SetDrumArticulation()
{
	CRegion * pRegion = GetHead();
	for( ; pRegion; pRegion = pRegion->GetNext())
	{
		pRegion->m_bUseInstrumentArticulation = false;
	}
}

void CInstrumentRegions::ValidateUIA()
{
	CRegion * pRegion = GetHead();
	for( ; pRegion; pRegion = pRegion->GetNext())
	{
		if(!pRegion->ValidateUIA())
		{
			if (m_pInstrument->m_rInstHeader.Locale.ulBank & F_INSTRUMENT_DRUMS)
			{
				pRegion->CreateAndInsertArticulationList();
			}
			else
			{
				m_pInstrument->InsertChildNode(NULL);
			}
		}
	}
}

CRegion* CInstrumentRegions::FindRegionFromMap(short nLayer, short nStartNote)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CRegion* pRegion = GetHead();
	CRegion* pTempRegion = NULL;

	// Walk the region list to find region
	for(; pRegion; pRegion = pRegion->GetNext())
	{
		if(pRegion->GetLayer() == nLayer && pRegion->m_rRgnHeader.RangeKey.usLow == nStartNote)
		{
			return pRegion;
		}
	}
	
	return NULL;
}

CRegion* CInstrumentRegions::FindRegionFromMap(int nNote)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CRegion* pRegion = GetHead();
	CRegion* pTempRegion = NULL;

	// Walk the region list to find region
	for(; pRegion; pRegion = pRegion->GetNext())
	{
		if(pRegion->m_rRgnHeader.RangeKey.usLow <= nNote && pRegion->m_rRgnHeader.RangeKey.usHigh >= nNote)
		{
			return pRegion;
		}
	}
	
	return NULL;
}

HRESULT CInstrumentRegions::FindRegionsForNote(short nNote, CPtrList* plstRegions)
{
	ASSERT(plstRegions);
	if(plstRegions == NULL)
	{
		return E_POINTER;
	}

	CRegion* pRegion = GetHead();
	for(; pRegion; pRegion = pRegion->GetNext())
	{
		if(pRegion->m_rRgnHeader.RangeKey.usLow <= nNote && pRegion->m_rRgnHeader.RangeKey.usHigh >= nNote)
		{
			plstRegions->AddTail(pRegion);
		}
	}

	return S_OK;
}

HRESULT	 CInstrumentRegions::FindPlayingRegions(USHORT usPlayNote, USHORT usVelocity, CPtrList* plstRegions)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
	{
		return E_UNEXPECTED;
	}
	
	CInstrumentFVEditor* pIE = m_pInstrument->GetInstrumentEditor();
	if(pIE == NULL)
	{
		return E_FAIL;
	}

	USHORT usActiveLayer = pIE->GetActiveLayer();
	bool bAuditionMode = m_pInstrument->GetAuditionMode();

	bool bIgnoreLayer = bAuditionMode == AUDITION_MULTIPLE ? true : false;

	CRegion* pRegion = GetHead();
	while(pRegion)
	{
		if(pRegion->IsOkayToPlay(usActiveLayer, usPlayNote, usVelocity, bIgnoreLayer) == true)
		{
			plstRegions->AddTail(pRegion);
		}

		pRegion = pRegion->GetNext();
	}

	return S_OK;
}


int CInstrumentRegions::GetNumberOfLayers()
{
	return m_nNumberOfLayers;
}

void CInstrumentRegions::SetNumberOfLayers(int nLayers)
{
	m_nNumberOfLayers = nLayers;
}

BOOL CInstrumentRegions::IsLayerEmpty(int nLayer)
{
	CRegion* pRegion = GetHead();
	while(pRegion)
	{
		if(pRegion->GetLayer() == nLayer)
		{
			return FALSE;
		}

		pRegion = pRegion->GetNext();
	}

	return TRUE;
}

/* copies a region, optionally returns the new region in ppRegion */
HRESULT	CInstrumentRegions::CopyRegion(short nSrcLayer, short nSrcStartNote, short nCopyLayer, short nCopyStartNote,
	CRegion **ppRegion)
{
	CRegion* pRegion = m_pInstrument->m_Regions.FindRegionFromMap(nSrcLayer, nSrcStartNote);
	if(pRegion == NULL)
		return E_FAIL;

	short nSrcEndNote = pRegion->m_rRgnHeader.RangeKey.usHigh;
	short nCopyEndNote = nCopyStartNote + abs(nSrcEndNote - nSrcStartNote);
	nCopyEndNote = nCopyEndNote > 127 ? 127 : nCopyEndNote;
	
	CRegion* pCopiedRegion;
	if(FAILED(InsertRegion(NULL, nCopyLayer, nCopyStartNote, nCopyEndNote, false, &pCopiedRegion)))
		return E_FAIL;

	if (ppRegion)
		*ppRegion = pCopiedRegion;

	// Copy all the properties for the region
	return pCopiedRegion->CopyProperties(pRegion);
}

/* duplicates a region into a new layer, returning the new region in ppRegion */
HRESULT CInstrumentRegions::CopyRegion(CRegion *pOldRegion, CRegion **ppNewRegion)
{
	ASSERT(m_pInstrument);

	// find first empty layer
	int nLayer = 0;
	while (!m_pInstrument->m_Regions.IsLayerEmpty(nLayer))
		nLayer++;

	CRegion* pNewRegion;
	if(FAILED(InsertRegion(NULL, nLayer, pOldRegion->GetRangeValue(false), pOldRegion->GetRangeValue(true), true, &pNewRegion)))
		return E_FAIL;
	ASSERT(pNewRegion);
	
	if (*ppNewRegion)
		*ppNewRegion = pNewRegion;

	// Copy all the properties for the region
	return pNewRegion->CopyProperties(pOldRegion);
}


/* get number of regions included in this collection, counting each mono channel separately */
LONG CInstrumentRegions::GetChannelCount()
{
	LONG cChannels = 0;
	CRegion* pRegion = GetHead();
	while (pRegion)
		{
		cChannels += pRegion->GetChannelCount();
		pRegion = pRegion->GetNext();
		}
	return cChannels;
}

