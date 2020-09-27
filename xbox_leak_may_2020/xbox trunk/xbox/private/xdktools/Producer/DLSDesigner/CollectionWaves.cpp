// CollectionWaves.cpp : implementation file
//

#include "stdafx.h"
#include "cderr.h"
#include "DlsDefsPlus.h"
#include "DLSDesignerDLL.h"
#include "DLSDesigner.h"
#include "Collection.h"
#include "Region.h"
#include "WaveNode.h"
#include "WaveDataManager.h"
#include "Wave.h"
#include "MonoWave.h"
#include "StereoWave.h"
#include "Instrument.h"
#include "resource.h"
#include "stream.h"
#include "InstrumentFVEditor.h"
#ifdef DMP_XBOX
#include "AdpcmLoopWarnDlg.h"
#endif // DMP_XBOX

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves constructor/destructor

CCollectionWaves::CCollectionWaves()
{
    m_dwRef = 1;
	m_pCollection = NULL;
	m_pDLSComponent = NULL;
	m_pIRootNode = NULL;
	m_pIParentNode = NULL;
}

CCollectionWaves::~CCollectionWaves()
{
	while (!IsEmpty())
	{
        CWaveNode* pWaveNode = RemoveHead();
        pWaveNode->Release();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves AList overrides


CWaveNode* CCollectionWaves::GetHead() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return(CWaveNode*)AList::GetHead();
}

CWaveNode* CCollectionWaves::RemoveHead() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return(CWaveNode*) AList::RemoveHead();
}

void CCollectionWaves::Remove(CWaveNode* pINode) 
{ 
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	AList::Remove((AListItem*) pINode);
}

void CCollectionWaves::AddTail(CWaveNode* pINode) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	AList::AddTail((AListItem*) pINode);
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves IUnknown implementation

HRESULT CCollectionWaves::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);    
	
	if( ::IsEqualIID(riid, IID_IDMUSProdNode)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = this;
        return S_OK;
    }
	else if(::IsEqualIID(riid, IID_IDMUSProdNodeDrop))
	{
		AddRef();
		*ppvObj = (IDMUSProdNodeDrop*)this;
		return S_OK;
	}

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CCollectionWaves::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CCollectionWaves::Release()
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
// CCollectionWaves IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves IDMUSProdNode::GetNodeImageIndex

HRESULT CCollectionWaves::GetNodeImageIndex(short* pnFirstImage)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pnFirstImage);
	
	ASSERT(m_pDLSComponent != NULL);

	return(m_pDLSComponent->GetFolderImageIndex(pnFirstImage));
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves IDMUSProdNode::GetComponent

HRESULT CCollectionWaves::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pDLSComponent != NULL );

	return m_pDLSComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves IDMUSProdNode::GetRootNode

HRESULT CCollectionWaves::GetDocRootNode( IDMUSProdNode** ppIRootNode )
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
// CCollectionWaves IDMUSProdNode::SetRootNode

HRESULT CCollectionWaves::SetDocRootNode( IDMUSProdNode* pIRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT( pIRootNode != NULL );

	m_pIRootNode = pIRootNode;
//	m_pIRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves IDMUSProdNode::GetParentNode

HRESULT CCollectionWaves::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT( m_pIParentNode != NULL );

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves IDMUSProdNode::SetParentNode

HRESULT CCollectionWaves::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves IDMUSProdNode::GetNodeId

HRESULT CCollectionWaves::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_WaveFolderNode;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves IDMUSProdNode::GetNodeName

HRESULT CCollectionWaves::GetNodeName(BSTR* pbstrName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	CString strName;
	TCHAR achBuffer[BUFFER_128];

	if(::LoadString(theApp.m_hInstance, IDS_WAVE_FOLDER_NAME, achBuffer, BUFFER_128))
	{
		strName = achBuffer;
	}

    *pbstrName = strName.AllocSysString();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves IDMUSProdNode::GetNodeNameMaxLength

HRESULT CCollectionWaves::GetNodeNameMaxLength(short* pnMaxLength)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pnMaxLength);

	*pnMaxLength = -1; // Can't rename a Waves folder

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves IDMUSProdNode::ValidateNodeName

HRESULT CCollectionWaves::ValidateNodeName(BSTR bstrName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Just free bstrName; can't rename Waves folder
	::SysFreeString(bstrName);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves IDMUSProdNode::SetNodeName

HRESULT CCollectionWaves::SetNodeName(BSTR bstrName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	// Just free bstrName; can't rename a Waves folder
	::SysFreeString(bstrName);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves IDMUSProdNode::GetEditorClsId

HRESULT CCollectionWaves::GetEditorClsId( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't edit a Pattern folder
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves IDMUSProdNode::GetEditorTitle

HRESULT CCollectionWaves::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't edit a Pattern folder
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves IDMUSProdNode::GetEditorWindow

HRESULT CCollectionWaves::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't edit a Pattern folder
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves IDMUSProdNode::SetEditorWindow

HRESULT CCollectionWaves::SetEditorWindow( HWND hWndEditor )
{
	return E_NOTIMPL;	// Can't edit a Pattern folder
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves IDMUSProdNode::UseOpenCloseImages

HRESULT CCollectionWaves::UseOpenCloseImages(BOOL* pfUseOpenCloseImages)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pfUseOpenCloseImages);

	*pfUseOpenCloseImages = TRUE;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves IDMUSProdNode::GetRightClickMenuId

HRESULT CCollectionWaves::GetRightClickMenuId(HINSTANCE* phInstance, UINT* pnMenuId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pnMenuId);
	ASSERT(phInstance);

	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_WAVES_NODE_RMENU;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves IDMUSProdNode::OnRightClickMenuInit

HRESULT CCollectionWaves::OnRightClickMenuInit(HMENU hMenu)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves IDMUSProdNode::OnRightClickMenuSelect

HRESULT CCollectionWaves::OnRightClickMenuSelect(long lCommandId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = E_FAIL;

	switch(lCommandId)
	{
		case IDM_NEW_WAVE:
			hr = InsertChildNode(NULL);
			break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves IDMUSProdNode::DeleteChildNode

HRESULT CCollectionWaves::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pIChildNode);
	if(pIChildNode == NULL)
	{
		return E_POINTER;
	}

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

	if(GetCount() == 1)
	{
		// Check if there's any instrument with just one region that's refering to this wave
		CInstrument* pInstrument = m_pCollection->m_Instruments.GetHead();
		while(pInstrument)
		{
			CInstrumentRegions* pInstRegions = pInstrument->GetRegions();
			if(pInstRegions->GetCount() == 1)
			{
				return E_FAIL;
			}
			
			pInstrument = pInstrument->GetNext();
		}
	}

	// Remove node from Project Tree
	if( m_pDLSComponent->m_pIFramework->RemoveNode( pIChildNode, fPromptUser ) == S_FALSE )
	{
		return E_FAIL;
	}

	// Set flag so we know to save file 
	m_pCollection->SetDirtyFlag();

	CWaveNode* pWaveNode = dynamic_cast<CWaveNode*>(pIChildNode);
	ASSERT(pWaveNode);

	
	// Release the dummy instrument
	CWave* pWave = pWaveNode->GetWave();
	ASSERT(pWave);
	if(pWave)
	{
		pWave->ReleaseDummyInstruments();
	}

	// Go through all the instruments and release the wavenode that may be referenced...
	CInstrument* pInstrument = m_pCollection->m_Instruments.GetHead();
	while(pInstrument) 
    {
		CInstrumentRegions* pInstRegions = pInstrument->GetRegions();
		ASSERT(pInstRegions);
		CRegion* pRegion = pInstRegions->GetHead();
		while(pRegion)
		{
			if(pRegion->GetWaveNode() == pWaveNode)
			{
				CWaveNode* pNewNode = pWaveNode->GetNext();
				if(pNewNode == NULL)
				{
					pNewNode = (CWaveNode*)pWaveNode->GetPrev(NULL);
				}

				ASSERT(pNewNode);
				pRegion->SetWave(pNewNode->GetWave());

				pIFramework->RefreshNode(pRegion);
			}

			pRegion = pRegion->GetNext();
		}

		pInstrument = pInstrument->GetNext();
	}

	// Remove from list
	Remove(pWaveNode);
	pIChildNode->Release();

	NotifyInstrumentsWavesChanged();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves IDMUSProdNode::InsertChildNode

HRESULT CCollectionWaves::InsertChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pDLSComponent != NULL );
	ASSERT( m_pDLSComponent->m_pIFramework != NULL );
	ASSERT( m_pCollection != NULL );

	if( pIChildNode == NULL )
	{
        // Put up a file open dilaog.
        CHAR szFilter[39] = "Wave Files (*.wav;*.wvp)|*.wav;*.wvp||";
		CFileDialog FileDlg(TRUE, "WAV", NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT, szFilter, NULL);
		
		CString sFileNames;
		FileDlg.m_ofn.lpstrFile = sFileNames.GetBuffer(2048);
		FileDlg.m_ofn.nMaxFile = 2048; // Max filename buffer size for NT 4.0

		CString sDialogTitle;
		sDialogTitle.LoadString(IDS_INSERT_COLLECTION_WAVE_TITLE);
		FileDlg.m_ofn.lpstrTitle = sDialogTitle;

        if(FileDlg.DoModal() == IDOK)
        {
            // Openfilename succeeded.
			POSITION position = FileDlg.GetStartPosition();

			while(position)
			{
				CString sFileName = FileDlg.GetNextPathName(position);
				if(FAILED(InsertWaveFile(sFileName)))
				{
					CString sErrorMessage;
					sErrorMessage.Format( IDS_ERR_FAILED_WAVE_INSERT, sFileName);
					AfxMessageBox(sErrorMessage);
				}
			}
        }
        else
		{
			if(CommDlgExtendedError() == FNERR_BUFFERTOOSMALL)
				AfxMessageBox(IDS_ERR_TOOMANY_WAVE_FILES);

			return S_OK;
		}
    }
	else
	{
		pIChildNode->AddRef();
		if(AddToCollection(pIChildNode))
		{
			return S_OK;
		}
	}
	
	return E_FAIL;

}


HRESULT CCollectionWaves::InsertWaveFile(CString sFileName, CWaveNode** ppInsertedWave)
{
	IStream* pStream = NULL;
	HRESULT hr = S_OK;

    ASSERT(m_pDLSComponent);
    if(m_pDLSComponent == NULL)
    {
        return E_UNEXPECTED;
    }

    IDMUSProdConductor* pIConductor = m_pDLSComponent->m_pIConductor;
    if(pIConductor == NULL)
    {
        return E_UNEXPECTED;
    }

	// turn on wait cursor
	CWaitCursor waitCursor;
	
	// Create a stream from this filename.
	if(SUCCEEDED(AllocStreamFromFile(&pStream, sFileName.GetBuffer(0), 0)))
	{
		CWaveNode* pIChildNode = NULL;
		if(SUCCEEDED(hr = CWaveNode::CreateWaveNode(m_pDLSComponent, pStream, sFileName, &pIChildNode, this, true)))
		{
			CWave* pWave = pIChildNode->GetWave();
			ASSERT(pWave);

            pIChildNode->SetSourceInTempFile(TRUE);

			CString sWaveName = pWave->GetName();

			if (sWaveName.IsEmpty())
			{
				int nDirectoryPathLength = sFileName.ReverseFind(TCHAR('\\')) + 1;
				int nExtractionLength = sFileName.GetLength() - nDirectoryPathLength;
				
				// Take out the Directory Path									
				CString sName = sFileName.Right(nExtractionLength);

				// Take out the .ext appendage
				sName = sName.Left(nExtractionLength - 4);
				pWave->SetName(sName);
			}

			pWave->SetCollection(m_pCollection);

			if(FAILED(pIChildNode->InitAndDownload()))
			{
				pIChildNode->Release();
				if(pStream)
				{
					pStream->Release();
				}

				return E_FAIL;
			}

			if(!AddToCollection(pIChildNode))
			{
				if(pStream)
				{
					pStream->Release();
				}

				return E_FAIL;
			}

			// Register it with the transport
			pIChildNode->RegisterWithTransport();
            pIConductor->SetActiveTransport(pIChildNode, BS_PLAY_ENABLED | BS_NO_AUTO_UPDATE);

            if(ppInsertedWave != NULL)
			{
				*ppInsertedWave = (CWaveNode*)pIChildNode;
			}
		}
		
		if(pStream)
		{
			pStream->Release();
		}
	}
	else
		hr = E_FAIL;

	return hr;
}



BOOL CCollectionWaves::AddToCollection(IDMUSProdNode* pIChildNode)
{
	 // Add to the collection
	CWave* pWave = ((CWaveNode*)pIChildNode)->GetWave(); 
	ASSERT(pWave);
	if(pWave == NULL)
		return FALSE;

	/*
#ifdef DMP_XBOX
	if( pWave->IsLooped()
	&&	pWave->m_bCompressed
	&&	pWave->GetCompressionFormatTag() == XBOXADPCM_FORMAT_TAG )
	{
		WLOOP waveLoop = pWave->GetWLOOP();
		if( (waveLoop.ulStart % 64) != 0
		||	(waveLoop.ulLength % 64) != 0 )
		{
			if( !m_pDLSComponent->IsNoShowBadXboxLoopWarning() )
			{
				CAdpcmLoopWarnDlg* pAdpcmLoopWarnDlg = new CAdpcmLoopWarnDlg(m_pDLSComponent, pWave->m_rWaveformat.nSamplesPerSec, waveLoop.ulStart, waveLoop.ulLength, 0);
				int nRes = pAdpcmLoopWarnDlg->DoModal();
				delete pAdpcmLoopWarnDlg;
				if( IDOK == nRes )
				{
					// Modify the loop points
					waveLoop.ulStart = 64 * ((waveLoop.ulStart + 32) / 64);
					waveLoop.ulLength = 64 * ((waveLoop.ulLength + 32) / 64);
					pWave->SetLoop( waveLoop.ulStart, waveLoop.ulStart + waveLoop.ulLength );
				}
			}
		}
	}
#endif // DMP_XBOX
	*/


	pWave->SetCollection(m_pCollection);

	// add to CollectionWaves list
	AddTail((CWaveNode*)pIChildNode);

	// Set root and parent node of ALL children
	theApp.SetNodePointers( pIChildNode, (IDMUSProdNode *)m_pCollection, (IDMUSProdNode*)this );

	// Add node to Project Tree
	if( !SUCCEEDED ( m_pDLSComponent->m_pIFramework->AddNode(pIChildNode, (IDMUSProdNode*)this) ) )
	{
		DeleteChildNode( pIChildNode, FALSE );
		return FALSE;
	}

	// Set flag so we know to save file 
	m_pCollection->SetDirtyFlag();

	// Set the IDs for the newly added wave
	m_pCollection->SetWaveIDs();

	// Tell the instruments that the waves have changed
	NotifyInstrumentsWavesChanged();

	return TRUE;
}



////////////////////////////////////////////////////////////////////////////
// CCollectionWaves IDMUSProdNode::DeleteNode

HRESULT CCollectionWaves::DeleteNode( BOOL fPromptUser )
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
//	CCollectionWaves IDMUSProdNode::GetFirstChild
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionWaves::GetFirstChild(IDMUSProdNode** ppIFirstChildNode)
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
//	CCollectionWaves IDMUSProdNode::GetNextChild
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionWaves::GetNextChild(IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode)
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

	IDMUSProdNode* pINode = (IDMUSProdNode*) ((CWaveNode*) pIChildNode)->GetNext();

	if(pINode)
	{
		pINode->AddRef();
		*ppINextChildNode = pINode;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CCollectionWaves IDMUSProdNode::GetNodeListInfo
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionWaves::GetNodeListInfo(DMUSProdListInfo* pListInfo)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pListInfo);
	
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves IDMUSProdNode::OnNodeSelChanged

HRESULT CCollectionWaves::OnNodeSelChanged( BOOL fSelected )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
//
// CCollectionWaves IDMUSProdNode::CreateDataObject
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionWaves::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollectionWaves IDMUSProdNode::CanCut
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionWaves::CanCut()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return S_OK;

}

/////////////////////////////////////////////////////////////////////////////
//
// CCollectionWaves IDMUSProdNode::CanCopy
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionWaves::CanCopy()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return S_OK;

}

/////////////////////////////////////////////////////////////////////////////
//
// CCollectionWaves IDMUSProdNode::CanDelete
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionWaves::CanDelete()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return S_OK;

}

/////////////////////////////////////////////////////////////////////////////
//
// CCollectionWaves IDMUSProdNode::CanDeleteChildNode
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionWaves::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return S_OK;   // no children pass up this message but we say ok anyway

}

/////////////////////////////////////////////////////////////////////////////
//
// CCollectionWaves IDMUSProdNode::CanPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionWaves::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
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

	if(SUCCEEDED(pDataObject->IsClipFormatAvailable( pIDataObject, m_pDLSComponent->m_cfInstrument)))
        hr = S_OK;

	if(SUCCEEDED(pDataObject->IsClipFormatAvailable( pIDataObject, m_pDLSComponent->m_cfWave)))
        hr = S_OK;

	pDataObject->Release();

    return hr;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollectionWaves IDMUSProdNode::PasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionWaves::PasteFromData( IDataObject* pIDataObject )
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

	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_pDLSComponent->m_cfInstrument ) ) )
	{
		// Handle CF_INSTRUMENT format
		if(m_pCollection)
			hr = m_pCollection->m_Instruments.PasteCF_INSTRUMENT( pDataObject, pIDataObject);
		pDataObject->Release();
		return hr;
	}

	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_pDLSComponent->m_cfWave ) ) )
	{
		// Handle CF_WAVE format
		hr = PasteCF_WAVE( pDataObject, pIDataObject);
		pDataObject->Release();
		return hr;
	}

	pDataObject->Release();
    return hr;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollectionWaves IDMUSProdNode::CanChildPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionWaves::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
												 BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;   //children (waves) handle this themselves

}

/////////////////////////////////////////////////////////////////////////////
//
// CCollectionWaves IDMUSProdNode::ChildPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionWaves::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return PasteFromData(pIDataObject);

}

/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves IDMUSProdNode::GetObject

HRESULT CCollectionWaves::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
//
// Implementation of IDMUSProdNodeDrop methods
//
/////////////////////////////////////////////////////////////////////////////
HRESULT CCollectionWaves::OnDropFiles(HANDLE handleDropInfo)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HDROP hDropInfo = (HDROP)handleDropInfo;

	UINT nFiles = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);

	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp != NULL);
	for (UINT iFile = 0; iFile < nFiles; iFile++)
	{
		TCHAR szFileName[_MAX_PATH];
		::DragQueryFile(hDropInfo, iFile, szFileName, _MAX_PATH);
		if(FAILED(InsertWaveFile(szFileName)))
		{
			CString sErrorMessage;
			sErrorMessage.Format(IDS_ERR_FAILED_WAVE_INSERT, szFileName);
			AfxMessageBox(sErrorMessage);
		}
	}
	::DragFinish(hDropInfo);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// CCollectionWaves PasteCF_WAVE
//
//////////////////////////////////////////////////////////////////////
HRESULT CCollectionWaves::PasteCF_WAVE( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject)
{
	IStream* pStream;
	HRESULT hr = E_FAIL;

    if (SUCCEEDED(pDataObject->AttemptRead(pIDataObject,  m_pDLSComponent->m_cfWave, &pStream )))
    {
		LARGE_INTEGER liTemp;

		// Seek to beginning of stream
		liTemp.QuadPart = 0;
		pStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

		IDMUSProdRIFFStream* pIRiffStream = NULL;
		if(FAILED(AllocRIFFStream(pStream, &pIRiffStream)))
		{
			return E_FAIL;
		}
		
		hr = PasteWaves(pIRiffStream);

		pIRiffStream->Release();
		pStream->Release();
    }
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
// CCollectionWaves::Load

HRESULT CCollectionWaves::Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	IStream*		pIStream;
	MMCKINFO		ck;
    HRESULT			hr = S_OK;

	ASSERT( m_pDLSComponent != NULL );
	
    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	ck.ckid = 0;
	ck.fccType = 0;
    int count = 0;
	while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0)
	{
		switch (ck.ckid) {
		case FOURCC_LIST :
			switch (ck.fccType)
			{
				case mmioFOURCC('W','A','V','E') :
			    case mmioFOURCC('w','a','v','e') :
				{
					CWaveNode* pWaveNode = NULL;
					if(SUCCEEDED(CWaveNode::CreateWaveNode(m_pDLSComponent, this, pIRiffStream, &ck, &pWaveNode)))
					{
						if(IsNodeInCollection(pWaveNode) == FALSE)
						{
							AddTail(pWaveNode);
							pWaveNode->SetCollection(m_pCollection);
							
						}
					}
					else
					{
						pIStream->Release();
						return E_FAIL;
					}
					break;
				}
			}
		}

		theApp.m_dwProgressBarRemainingBytes -= ck.cksize;
		m_pDLSComponent->m_pIFramework->SetProgressBarPos( theApp.m_hKeyProgressBar,
					(theApp.m_dwProgressBarTotalBytes - theApp.m_dwProgressBarRemainingBytes) / theApp.m_dwProgressBarIncrement );

		pIRiffStream->Ascend( &ck, 0 );
		ck.ckid = 0;
		ck.fccType = 0;
	}
	pIStream->Release();
	return hr;
}

HRESULT CCollectionWaves::PasteWaves(IDMUSProdRIFFStream* pIRiffStream)
{
	ASSERT(pIRiffStream);
	if(pIRiffStream == NULL)
	{
		return E_POINTER;
	}

	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_FAIL;
	}

	MMCKINFO ckMain;
	ckMain.ckid = FOURCC_DRAG;
	ckMain.fccType = 0;
	while(pIRiffStream->Descend(&ckMain, NULL, MMIO_FINDCHUNK) == 0 )
	{
		CWaveNode* pWaveNode = NULL;
		
		char szTempPath[MAX_PATH];
		GetTempPath(MAX_PATH, szTempPath);
		char szTempFileName[MAX_PATH];

		CString sTempFileName = "";
		CString sPrefix = "DMP";
		CString sExt = "tmp";
		if(FAILED(CWaveDataManager::CreateUniqueTempFile(sPrefix, sExt, sTempFileName)))
		{
			GetTempFileName(szTempPath, sPrefix, 0, szTempFileName);
			sTempFileName = szTempFileName;
		}

		// Delete the temp file
		DeleteFile(szTempFileName);

		DWORD dwPosition = StreamTell(pIStream);
		StreamSeek(pIStream, (dwPosition - 8), STREAM_SEEK_SET);

		// Ascend happens in the called method
		if(FAILED(CWaveNode::CreateWaveNode(m_pDLSComponent, pIStream, sTempFileName, &pWaveNode, this, true)))
		{
			pIStream->Release();
			return E_FAIL;
		}

		// NOTENOTE!! Ascends take care of the stream pointer....so we don't need this anymore
		// Take it out when fully tested....
		/*StreamSeek(pIStream, dwPosition, STREAM_SEEK_SET);*/

		// Add to the collection
		pWaveNode->SetCollection(m_pCollection);

		if(FAILED(pWaveNode->InitAndDownload()))
		{
			pIStream->Release();
			pWaveNode->Release();
			return E_FAIL;
		}

		// Add to Wave list
		AddTail(pWaveNode);

		// Set root and parent node of ALL children
		theApp.SetNodePointers( pWaveNode, (IDMUSProdNode *)m_pCollection, (IDMUSProdNode *)this);

		// Add node to Project Tree
		if(FAILED(m_pDLSComponent->m_pIFramework->AddNode(pWaveNode, (IDMUSProdNode*)this)))
		{	
			pIStream->Release();
			DeleteChildNode( pWaveNode, FALSE );
			return E_FAIL;
		}

		// Register it with the transport
		pWaveNode->RegisterWithTransport();
	}

	// Set flag so we know to save file 
	m_pCollection->SetDirtyFlag();

	// set the wave IDs
	m_pCollection->SetWaveIDs();

	// Tell the instruments to update their editors
	NotifyInstrumentsWavesChanged();

	pIStream->Release();

	return S_OK;

}

void CCollectionWaves::NotifyInstrumentsWavesChanged()
{
	CInstrument * pInstrument = m_pCollection->m_Instruments.GetHead();
	while (pInstrument) // for all instruments go thru all regions
    {
        CInstrumentFVEditor* pInstrumentEditor = pInstrument->GetInstrumentEditor();
        if(pInstrumentEditor)
        {
			pInstrumentEditor->DeleteAndSetupWavesForCurrentRegion();
        }

		pInstrument = pInstrument->GetNext();
    }
}

void CCollectionWaves::ReplaceWave(CWaveNode* pWaveNode, CString sWaveFileName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pWaveNode);
	ASSERT(!sWaveFileName.IsEmpty());
	if(pWaveNode== NULL || sWaveFileName.IsEmpty())
    {
		return;
    }

    ASSERT(m_pDLSComponent);
    if(m_pDLSComponent == NULL)
    {
        return;
    }

    IDMUSProdFramework* pIFramework = m_pDLSComponent->m_pIFramework;
    ASSERT(pIFramework);
    if(pIFramework == NULL)
    {
        return;
    }

	// Are you sure you want to replace this file?
	if(AfxMessageBox(IDS_CONFIRM_REPLACE_WAVE, MB_YESNO) == IDNO)
		return;
	
	CWaveNode* pNewWaveNode = NULL;
	if(FAILED(InsertWaveFile(sWaveFileName, &pNewWaveNode)) || pNewWaveNode == NULL)
		return;

	CWave* pOldWave = pWaveNode->GetWave();
	ASSERT(pOldWave);

	CWave* pNewWave = pNewWaveNode->GetWave();
	ASSERT(pNewWave);

	CMonoWave* pLeftChannelWave;
	CMonoWave* pRightChannelWave;
	
	CMonoWave* pOldLeftChannelWave;
	CMonoWave* pOldRightChannelWave;

	bool bOldStereo = pOldWave->IsStereo();
	if (bOldStereo)
	{
		pOldLeftChannelWave = ((CStereoWave*)pOldWave)->GetLeftChannel();
		pOldRightChannelWave = ((CStereoWave*)pOldWave)->GetRightChannel();
	}
#ifdef _DEBUG
	else
		pOldLeftChannelWave = pOldRightChannelWave = (CMonoWave *)0xDEAD;
#endif

	bool bStereo = pNewWave->IsStereo();
	if (bStereo)
	{
		pLeftChannelWave = ((CStereoWave*)pNewWave)->GetLeftChannel();
		pRightChannelWave = ((CStereoWave*)pNewWave)->GetRightChannel();
	}
#ifdef _DEBUG
	else
		pLeftChannelWave = pRightChannelWave = (CMonoWave *)0xDEAD;
#endif

	// Update all regions that reference this wave
	CInstrument* pInstrument = m_pCollection->m_Instruments.GetHead();
	while (pInstrument) // for all instruments go thru all regions
    {
        CRegion* pRegion = pInstrument->m_Regions.GetHead();
        while (pRegion)
        {
			// figure out what wave to replace with, if any
			CWave *pReplaceWave = NULL;
			if (pRegion->m_pWave == pOldWave)
				pReplaceWave = pNewWave; // replace with new, irrespective of number of channels in new wave
			else if (bOldStereo)
				{
				// if left channel, replace with new mono, or left channel if stereo
				if (pRegion->m_pWave == pOldLeftChannelWave)
					pReplaceWave = bStereo ? pLeftChannelWave : pNewWave;

				// if right channel, replace with new mono, or right channel if stereo
				if (pRegion->m_pWave == pOldRightChannelWave)
					pReplaceWave = bStereo ? pRightChannelWave : pNewWave;
				}

            if (pReplaceWave != NULL)
				{
				pRegion->SetWave(pReplaceWave);
                pRegion->GetName();
                pIFramework->RefreshNode(pRegion);
				pInstrument->RefreshUI(true);
				}
			
			pRegion = pRegion->GetNext();
        }
		
		pInstrument = pInstrument->GetNext();
    }
    
    // Unregister the wave from the transport
    pWaveNode->UnRegisterFromTransport();

	// Now delete the original wave
	DeleteChildNode((IDMUSProdNode*)pWaveNode, FALSE);

	// Mark the collection as dirty so we get the save prompt
	m_pCollection->SetDirtyFlag();
}

HRESULT CCollectionWaves::GetListOfMonoWaves(CPtrList* pWaveList)
{
	ASSERT(pWaveList);
	if(pWaveList == NULL)
		return E_POINTER;

	CWaveNode* pWaveNode = GetHead();
	while(pWaveNode)
	{
		CWave* pWave = pWaveNode->GetWave();
		ASSERT(pWave);
		if(pWave->IsStereo())
		{
			CStereoWave* pStereoWave = (CStereoWave*)pWave;
			pWaveList->AddTail(pStereoWave->GetLeftChannel());
			pWaveList->AddTail(pStereoWave->GetRightChannel());
		}
		else
		{
			pWaveList->AddTail((CMonoWave*)pWave);
		}

		pWaveNode = pWaveNode->GetNext();
	}

	return S_OK;
}

HRESULT CCollectionWaves::GetListOfWaves(CPtrList* pWaveList)
{
	ASSERT(pWaveList);
	if(pWaveList == NULL)
		return E_POINTER;

	CWaveNode* pWaveNode = GetHead();
	while(pWaveNode)
	{
		CWave* pWave = pWaveNode->GetWave();
		ASSERT(pWave);
		if(pWave)
		{
			pWaveList->AddTail((CWave*)pWave);
		}

		pWaveNode = pWaveNode->GetNext();
	}

	return S_OK;
}

/* Returns wave pointed to by first node, NULL if none */
CWave*	CCollectionWaves::GetFirstWave()
{
	CWaveNode* pWaveNode = GetHead();
	if (!pWaveNode)
		return NULL;

	CWave* pWave = pWaveNode->GetWave();
	ASSERT(pWave);
	return pWave;
}

BOOL CCollectionWaves::IsNodeInCollection(CWaveNode* pWaveNode)
{
	CWaveNode* pNode = GetHead();
	
	while(pNode)
	{

		if(pWaveNode == pNode)
			return TRUE;
		
		pNode = pNode->GetNext();
	}

	return FALSE;
}

CCollection* CCollectionWaves::GetCollection()
{
	return m_pCollection;
}

void CCollectionWaves::ReleaseWaveNodeDummyInstruments()
{
	CWaveNode* pWaveNode = GetHead();
	while(pWaveNode)
	{
		CWave* pWave = pWaveNode->GetWave();
		ASSERT(pWave);
		if(pWave)
		{
			pWave->ReleaseDummyInstruments();
		}

		pWaveNode = pWaveNode->GetNext();
	}
}

void CCollectionWaves::UnRegisterWavesFromTransport()
{
	ASSERT(m_pDLSComponent);
	if(m_pDLSComponent == NULL)
	{
		return;
	}

	IDMUSProdConductor* pIConductor = m_pDLSComponent->m_pIConductor;
	ASSERT(pIConductor);
	if(pIConductor == NULL)
	{
		return;
	}

	CWaveNode* pWaveNode = GetHead();
	while(pWaveNode)
	{
		pWaveNode->UnRegisterFromTransport();
		pWaveNode = pWaveNode->GetNext();
	}
}

void CCollectionWaves::RegisterWavesWithTransport()
{
	ASSERT(m_pDLSComponent);
	if(m_pDLSComponent == NULL)
	{
		return;
	}

	IDMUSProdConductor* pIConductor = m_pDLSComponent->m_pIConductor;
	ASSERT(pIConductor);
	if(pIConductor == NULL)
	{
		return;
	}

	CWaveNode* pWaveNode = GetHead();
	while(pWaveNode)
	{
		pWaveNode->RegisterWithTransport();
		pWaveNode = pWaveNode->GetNext();
	}
}
