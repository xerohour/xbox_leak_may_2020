//////////////////////////////////////////////////////////////////////
//
// WaveNode.cpp : implementation file
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <mmsystem.h>
#include <mmreg.h>
#include <msacm.h>
#include "DLSDesignerDLL.h"
#include "WaveNode.h"
#include "Wave.h"
#include "WaveDataManager.h"
#include "MonoWave.h"
#include "StereoWave.h"
#include "WaveStream.h"
#include "NewWaveDialog.h"
#include "Collection.h"
#include "CollectionWaves.h"
#include "DLSLoadSaveUtils.h"
#include "stream.h"
#include "Instrument.h"
#include "Wavectl.h"
#include "Timeline.h"
#include "DLSLoadSaveUtils.h"
#include <WaveTimelineDraw.h>
#include "dmusicf.h"
#include "DlsDefsPlus.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
//
//	CWaveNode::CWaveNode();
//
//////////////////////////////////////////////////////////////////////
CWaveNode::CWaveNode(CDLSComponent* pComponent, bool bStereo) : m_pWaveCtrl(NULL), 
m_bStereo(bStereo),
m_bPastingWave(false),
m_bDragCopy(false),
m_pIDirectSoundWave(NULL),
m_pDirectSoundWaveStream(NULL),
m_bInCollection(false),
m_bIsDirty(FALSE),
m_bSrcInATempFile(FALSE),
m_hStopEvent(0)
{
	ASSERT( pComponent != NULL );

    m_lRef = 0;
	AddRef();

	m_pComponent = pComponent;
	m_pComponent->AddRef();

	m_pWave = NULL;
	m_pIRootNode = NULL;
	m_pIParentNode = NULL;
	m_hWndEditor = NULL;

	::CoCreateGuid(&m_guidWaveNode);

	::CoCreateGuid(&m_guidFile);

	::CoCreateGuid(&m_guidVersion);

	m_hStopEvent = ::CreateEvent(NULL, TRUE, FALSE, "Oneshot Timeout");
	
	InitializeCriticalSection(&m_CriticalSection);
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode::~CWaveNode()
//
//////////////////////////////////////////////////////////////////////
CWaveNode::~CWaveNode()
{
	ASSERT(m_pComponent);

	::CloseHandle(m_hStopEvent);

	if(m_pIDirectSoundWave)
	{
		m_pIDirectSoundWave->Release();
		m_pIDirectSoundWave = NULL;
	}

	if(m_pDirectSoundWaveStream)
	{
		m_pDirectSoundWaveStream->Release();
		m_pDirectSoundWaveStream = NULL;
	}

	if(m_pWave)
	{
		m_pWave->Release();
	}

	if(m_pComponent)
	{
		m_pComponent->Release();
	}

	DeleteCriticalSection(&m_CriticalSection);
}

//////////////////////////////////////////////////////////////////////
//
//	CWavenode::CreateWaveNode
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::CreateWaveNode(CDLSComponent* pComponent, CWaveNode** ppWaveNode)
{
	// Must have a component
	ASSERT(pComponent);
	if(pComponent == NULL)
		return E_POINTER;

	*ppWaveNode = NULL;

	// Throw up a dialog asking for the format
	CNewWaveDialog newWaveDlg;
	if(newWaveDlg.DoModal() == IDOK)
	{
		// Mono or stereo?
		int nChannels = 1;
		if(newWaveDlg.m_bLastSelectedStereo)
			nChannels = 2;

		// 8 bit or 16 bit?
		int nSampleSize = 16;
		if(newWaveDlg.m_bLastSelected8Bit)
			nSampleSize = 8;

		// What's the sample rate?
		DWORD dwSampleRate = atoi(newWaveDlg.m_sLastSelectedRate);

		// Get a temp file which we'll rename later
		// Get a temp file name
		char szTempPath[MAX_PATH];
		DWORD dwSuccess = GetTempPath(MAX_PATH, szTempPath);
		CString sTempPath = szTempPath;
		
		char szTempName[MAX_PATH];
		CString sTempName;
		
		CString sPrefix = "DMP";
		CString sExtension = "tmp";
		if(FAILED(CWaveDataManager::CreateUniqueTempFile(sPrefix, sExtension, sTempName)))
		{
			GetTempFileName(sTempPath, sPrefix, 0, szTempName);
			sTempName = szTempName;
		}

		// Create a new wave node
		CWaveNode* pWaveNode = NULL;
		if(nChannels == 1)
		{
			pWaveNode = new CWaveNode(pComponent, false);
			if(pWaveNode == NULL)
			{
				return E_OUTOFMEMORY;
			}

			pWaveNode->m_sFileName = sTempName;
			pWaveNode->m_pWave = new CMonoWave(pWaveNode, dwSampleRate, nSampleSize);
		}
		else if(nChannels == 2)
		{
			pWaveNode = new CWaveNode(pComponent, true);
			if(pWaveNode == NULL)
			{
				return E_OUTOFMEMORY;
			}

			pWaveNode->m_sFileName = sTempName;
			pWaveNode->m_pWave = new CStereoWave(pWaveNode, true, dwSampleRate, nSampleSize);
		}

		if(pWaveNode->m_pWave == NULL)
			return E_OUTOFMEMORY;

		*ppWaveNode = pWaveNode;

		return S_OK;
	}

	return E_FAIL;
}


//////////////////////////////////////////////////////////////////////
//
//	CWavenode::CreateWaveNode
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::CreateWaveNode(CDLSComponent* pComponent, CCollectionWaves* pWavesCollection, IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain, CWaveNode** ppWaveNode)
{
	ASSERT(pWavesCollection);
	if(pWavesCollection == NULL)
		return E_POINTER;

	// Must have a component
	ASSERT(pComponent);
	if(pComponent == NULL)
		return E_POINTER;

	// Must have a stream to load the wave from
	ASSERT(pIRiffStream);
	if(pIRiffStream == NULL)
		return E_POINTER;

	ASSERT(pckMain);
	if(pckMain == NULL)
		return E_POINTER;


	CWaveNode* pWaveNode = NULL;
	GUID guidStereoWave;
	::CoCreateGuid(&guidStereoWave);
	
	// Keep this in a temporary guid var
	GUID guidTemp;
	CopyGUID(guidStereoWave, guidTemp);

	bool bStereoWave= false;
	bool bTrueStereo = false;
	bool bRightChannel = false;
	
	// Find out if we already have partially loaded this stereo wave node
	if(SUCCEEDED(IsLoadingStereoWave(pIRiffStream, pckMain, bStereoWave, bTrueStereo, guidStereoWave, bRightChannel)))
	{
		if(bStereoWave && !bTrueStereo)
		{
			pWaveNode  = FindNodeForStereoGuid(pWavesCollection, guidStereoWave);
		}
	}

	CCollection* pCollection = pWavesCollection->GetCollection();
	ASSERT(pCollection);
	if(pCollection == NULL)
	{
		return E_FAIL;
	}

	CString sCollectionFileName = pCollection->GetNodeFileName();
	if(sCollectionFileName.IsEmpty())
	{
		// Get the name from the passed strream...at least it should know what file it's loading from right?
		// This is probably a collection embedded in a container....
		IStream* pIStream = pIRiffStream->GetStream();
		ASSERT(pIStream);
		if(pIStream == NULL)
		{
			return E_FAIL;
		}

		CString sFilePath;
		IDMUSProdPersistInfo* pIJazzPersistInfo;
		if(SUCCEEDED(pIStream->QueryInterface(IID_IDMUSProdPersistInfo,(void**) &pIJazzPersistInfo)))
		{
			BSTR bstrFileName;
			pIJazzPersistInfo->GetFileName(&bstrFileName);
			sCollectionFileName = bstrFileName;
			SysFreeString(bstrFileName);
			pIJazzPersistInfo->Release();

			// Set the name for the collection node
			pCollection->SetNodeFileName(sCollectionFileName);
		}
		else
		{
			pIStream->Release();
			return E_FAIL;
		}
		
		pIStream->Release();
	}
	
	// We don't so get one
	if(pWaveNode == NULL)
	{
		pWaveNode = new CWaveNode(pComponent);
		if(pWaveNode == NULL)
		{
			return E_OUTOFMEMORY;
		}
	}

	pWaveNode->SetFileName(sCollectionFileName);
	pWaveNode->m_bInCollection = true;

	if(SUCCEEDED(pWaveNode->Load(pIRiffStream, pckMain)))
	{
		pWaveNode->SetCollection(pCollection);
		*ppWaveNode = pWaveNode;
		return S_OK;
	}

	delete pWaveNode;
	*ppWaveNode = NULL;

	return E_FAIL;
}


// This method is called ONLY when inserting a NEW wave so we don't want to check for exisiting nodes
HRESULT CWaveNode::CreateWaveNode(CDLSComponent* pComponent, IStream* pIStream, CString sFileName, CWaveNode** ppWaveNode, CCollectionWaves* pWavesCollection, bool bInACollection)
{
	// Must have a component
	ASSERT(pComponent);
	if(pComponent == NULL)
	{
		return E_POINTER;
	}

	// Must have a stream to load the wave from
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_POINTER;
	}

    CCollection* pCollection = NULL;
    if(bInACollection)
    {
        ASSERT(pWavesCollection);
        if(pWavesCollection == NULL)
        {
            return E_INVALIDARG;
        }

        pCollection = pWavesCollection->GetCollection();
        ASSERT(pCollection);
        if(pCollection == NULL)
        {
            return E_UNEXPECTED;
        }
    }

	CWaveNode* pWaveNode = new CWaveNode(pComponent);
	if(pWaveNode == NULL)
	{
		return E_OUTOFMEMORY;
	}

    pWaveNode->m_bInCollection = bInACollection;
	pWaveNode->SetFileName(sFileName);

	if(SUCCEEDED(pWaveNode->Load(pIStream)))
	{
        pWaveNode->SetCollection(pCollection);
		*ppWaveNode = pWaveNode;
		return S_OK;
	}

	delete pWaveNode;
	*ppWaveNode = NULL;

	return E_FAIL;
}

CWaveNode* CWaveNode::FindNodeForStereoGuid(CCollectionWaves* pWavesCollection, const GUID& guidStereoWave)
{
	if(pWavesCollection == NULL)
		return NULL;

	CWaveNode* pWaveNode = pWavesCollection->GetHead();
	
	while(pWaveNode)
	{
		if(pWaveNode->IsStereo())
		{
			CStereoWave* pWave = (CStereoWave*) pWaveNode->GetWave();
			if(pWave->GetGUID() == guidStereoWave)
			{
				return pWaveNode;
			}
		}
		
		pWaveNode = pWaveNode->GetNext();
	}

	return NULL;
}


void CWaveNode::CopyGUID(const GUID& guidSrc, GUID& guidDest)
{
	guidDest.Data1 = guidSrc.Data1;
	guidDest.Data2 = guidSrc.Data2;
	guidDest.Data3 = guidSrc.Data3;
	CopyMemory(&(guidDest.Data4), &(guidSrc.Data4), sizeof(BYTE) * 8);
}

//////////////////////////////////////////////////////////////////////
//
//	CWavenode::QueryInterface()
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    if(::IsEqualIID(riid, IID_IDMUSProdNode)
	|| ::IsEqualIID(riid, IID_IUnknown))
    {
        AddRef();
        *ppvObj = (IDMUSProdNode*)this;
        return S_OK;
    }

    if(::IsEqualIID(riid, IID_IPersist))
    {
        AddRef();
        *ppvObj = (IPersist*)this;
        return S_OK;
    }

    if(::IsEqualIID(riid, IID_IPersistStream))
    {
        AddRef();
        *ppvObj = (IPersistStream*)this;
        return S_OK;
    } 

	if(::IsEqualIID(riid, IID_IDMUSProdWaveTimelineDraw))
    {
        AddRef();
        *ppvObj = (IDMUSProdWaveTimelineDraw*)this;
        return S_OK;
    } 

	if(::IsEqualIID(riid, IID_IDMUSProdPropPageObject))
    {
        m_pWave->AddRef();
        *ppvObj = (IDMUSProdPropPageObject*)m_pWave;
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
//	CWaveNode::AddRef()
//
//////////////////////////////////////////////////////////////////////
ULONG CWaveNode::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	AfxOleLockApp();
    return InterlockedIncrement(&m_lRef);
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode::Release()
//
//////////////////////////////////////////////////////////////////////
ULONG CWaveNode::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT(m_lRef != 0);

	InterlockedDecrement(&m_lRef); 

    if(m_lRef == 0)
    {
        delete this;
        return 0;
    }
	AfxOleUnlockApp();
    return m_lRef;
}

/////////////////////////////////////////////////////////////////////////////
// CWaveNode IDMUSProdNode implementation

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IDMUSProdNode::GetNodeImageIndex
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::GetNodeImageIndex(short* pnFirstImage)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pnFirstImage);
	
	ASSERT(m_pComponent != NULL);

	if(IsStereo())
	{
		return(m_pComponent->GetStereoWaveImageIndex(pnFirstImage));
	}

	return(m_pComponent->GetWaveImageIndex(pnFirstImage));
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IDMUSProdNode::GetComponent
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::GetComponent(IDMUSProdComponent** ppIComponent)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );

	return m_pComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IDMUSProdNode::GetDocRootNode
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::GetDocRootNode(IDMUSProdNode** ppIRootNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_pIRootNode)
	{
		m_pIRootNode->AddRef();
		*ppIRootNode = m_pIRootNode;
		return S_OK;
	}

	*ppIRootNode = NULL;
	return E_FAIL;

}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IDMUSProdNode::SetDocRootNode
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::SetDocRootNode(IDMUSProdNode* pIRootNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(pIRootNode != NULL);

	m_pIRootNode = pIRootNode;
//	m_pIRootNode->AddRef();		intentionally missing

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IDMUSProdNode::GetParentNode
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::GetParentNode(IDMUSProdNode** ppIParentNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIParentNode == NULL )
	{
		return E_POINTER;
	}

	*ppIParentNode = m_pIParentNode;

	if(m_pIParentNode)
	{
		m_pIParentNode->AddRef();
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IDMUSProdNode::SetParentNode
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::SetParentNode(IDMUSProdNode* pIParentNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IDMUSProdNode::GetNodeId
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::GetNodeId(GUID* pguid)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_WaveNode;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IDMUSProdNode::GetNodeName
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::GetNodeName(BSTR* pbstrName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(pbstrName);
    
	*pbstrName = m_pWave->GetName().AllocSysString();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IDMUSProdNode::GetNodeNameMaxLength
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::GetNodeNameMaxLength(short* pnMaxLength)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pnMaxLength);

	*pnMaxLength = BUFFER_256;	// Can't edit Wave name.

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IDMUSProdNode::ValidateNodeName
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::ValidateNodeName(BSTR bstrName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;

	strName = bstrName;
	
	::SysFreeString(bstrName);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CWavenode IDMUSProdNode::SetNodeName
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::SetNodeName(BSTR bstrName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_FAIL;
	}

	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		return E_FAIL;
	}

	ASSERT(m_pComponent->m_pIFramework);
	if(m_pComponent->m_pIFramework == NULL)
	{
		return E_FAIL;
	}

	
    CString strName = bstrName;
	::SysFreeString( bstrName );

	m_pWave->SetName(strName);
	SetTransportName();

	// Send the name change notification
	if(m_pComponent->m_pIFramework)
	{
		m_pComponent->m_pIFramework->NotifyNodes(this, WAVENODE_NameChange, NULL);
	}


	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IDMUSProdNode::GetEditorClsId
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::GetEditorClsId(CLSID* pClsId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    *pClsId = CLSID_Wave;
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IDMUSProdNode::GetEditorTitle
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::GetEditorTitle(BSTR* pbstrTitle)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strTitle;

	strTitle = _T("Wave: ");
	strTitle += m_pWave->GetName();

    *pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IDMUSProdNode::GetEditorWindow
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::GetEditorWindow(HWND* hWndEditor)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*hWndEditor = m_hWndEditor;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IDMUSProdNode::SetEditorWindow
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::SetEditorWindow(HWND hWndEditor)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_hWndEditor = hWndEditor;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IDMUSProdNode::UseOpenCloseImages
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::UseOpenCloseImages(BOOL* pfUseOpenCloseImages)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pfUseOpenCloseImages);

	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IDMUSProdNode::GetRightClickMenuId
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::GetRightClickMenuId(HINSTANCE* phInstance, UINT* pnMenuId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pnMenuId);
	ASSERT(phInstance);

	*phInstance = theApp.m_hInstance;
	*pnMenuId = IDM_WAVE_NODE_RMENU;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IDMUSProdNode::OnRightClickMenuInit
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::OnRightClickMenuInit(HMENU hMenu)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_UNEXPECTED;
	}

	if(IsInACollection())
	{
		CCollection* pCollection = m_pWave->GetCollection();
		ASSERT(pCollection);
		if(pCollection)
		{
			DWORD dwMenuFlags = MF_BYCOMMAND | MF_ENABLED;
			DWORD dwWaveNodes = pCollection->WaveNodeCount();
			DWORD dwInstrumentCount = pCollection->InstrumentCount();
			if(dwWaveNodes == 1 && dwInstrumentCount >= 1)
			{
				dwMenuFlags = MF_GRAYED | MF_BYCOMMAND;
			}

			CMenu menu;
 			if(menu.Attach(hMenu))
			{
				menu.EnableMenuItem(IDM_DELETE, dwMenuFlags);
				menu.Detach();
			}
		}
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IDMUSProdNode::OnRightClickMenuSelect
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::OnRightClickMenuSelect(long lCommandId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pComponent != NULL);
	ASSERT(m_pComponent->m_pIFramework != NULL);

	HRESULT hr = E_FAIL;
	IDMUSProdNode* pIParentNode;
	
	switch(lCommandId)
	{
		case IDM_OPEN:
			if(SUCCEEDED(m_pComponent->m_pIFramework->OpenEditor(this)))
			{
				hr = S_OK;
			}
			break;
		
        case IDM_SAVEAS:
            {
				hr = E_FAIL;
               if(m_pWave)
			   {
				   hr = m_pWave->SaveAs(NULL, true);
			   }
            }
    	break;

        case IDM_CUT:
			hr = E_NOTIMPL;			
			break;

		case IDM_COPY:
			hr = E_NOTIMPL;
			break;

		case IDM_PASTE:
			hr = E_NOTIMPL;
			break;

		case IDM_REPLACE:
			{
				if(m_pWave)
					m_pWave->Replace();
				break;
			}

		case IDM_DELETE:
			if ( ConfirmWaveDelete() )
			{
				if(SUCCEEDED(GetParentNode(&pIParentNode)))
				{
                    if(pIParentNode)
                    {
					    if(SUCCEEDED(pIParentNode->DeleteChildNode((IDMUSProdNode *)this, FALSE)))
					    {
						    hr = S_OK;				
					    }
					    pIParentNode->Release();
                    }
                    else if(pIParentNode == NULL)
                    {
                        hr = DeleteNode(false);
                    }
				}
			}
			break;

		case IDM_RENAME:
			if(SUCCEEDED(m_pComponent->m_pIFramework->EditNodeLabel((IDMUSProdNode *)this)))
			{
    			hr = S_OK;
			}
			break;
		
		case IDM_PROPERTIES:
			if(m_pWave)
			{
				hr = E_FAIL;
				if(SUCCEEDED(m_pWave->OnShowProperties()))
				{
					hr = S_OK;
				}
			}
			break;
	}

	return hr;

}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IDMUSProdNode::DeleteChildNode
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::DeleteChildNode(IDMUSProdNode* pIChildNode, BOOL fPromptUser)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_FAIL;	// Not implemented.
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IDMUSProdNode::InsertChildNode
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::InsertChildNode(IDMUSProdNode* pIChildNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_FAIL;	// Not implemented.
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IDMUSProdNode::DeleteNode
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::DeleteNode( BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_UNEXPECTED;
	}

	IDMUSProdNode* pIParentNode = NULL;

	bool bDelete = true;
	if(IsInACollection())
	{
		CCollection* pCollection = m_pWave->GetCollection();
		CCollectionInstruments* pInstruments = pCollection->GetInstruments();
		DWORD dwWaveCount= pCollection->WaveNodeCount();
		DWORD dwInstrumentCount = pCollection->InstrumentCount();

		if(dwWaveCount == 1 && dwInstrumentCount == 1)
		{
			return E_FAIL;
		}
	}

	if(fPromptUser)
	{
		bDelete = ConfirmWaveDelete();
	}

	if(bDelete)
	{
		// Release the dummy instrument
		m_pWave->ReleaseDummyInstruments();

		// Remove yourself from the components wavenode list
		// You will not be in that list if you're in a collection 
		m_pComponent->DeleteFromWaveNodesList(this);

		UnRegisterFromTransport();

		if(SUCCEEDED(GetParentNode(&pIParentNode)))
		{
			if(pIParentNode)
			{
				HRESULT hr = pIParentNode->DeleteChildNode( this, FALSE );
				pIParentNode->Release();
				return hr;		
			}
		}

		// We need to close out all the open file handles
		if(FAILED(m_pWave->CloseSourceHandler()))
		{
			return E_FAIL;
		}

		// No parent so we delete ourself
		if(m_pComponent->m_pIFramework->RemoveNode(this, fPromptUser) == S_FALSE)
		{
			return E_FAIL;	
		}
		
		return S_OK;
	}

	return E_FAIL;
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IDMUSProdNode::GetFirstChild
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::GetFirstChild(IDMUSProdNode** ppIFirstChildNode)
{
	// A Wave does not have any children	
	
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IDMUSProdNode::GetNextChild
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::GetNextChild(IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode)
{
	// A Wave does not have any children	
	
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IDMUSProdNode::GetNodeListInfo
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::GetNodeListInfo(DMUSProdListInfo* pListInfo)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pListInfo == NULL )
	{
		return E_POINTER;
	}

	CString strDescriptor;
	CString strName = m_pWave->GetName();

    pListInfo->bstrName = strName.AllocSysString();
    pListInfo->bstrDescriptor = strDescriptor.AllocSysString();
	memcpy( &pListInfo->guidObject, &m_guidFile, sizeof(GUID) );

	// Must check pListInfo->wSize before populating additional fields
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IDMUSProdNode::OnNodeSelChanged
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::OnNodeSelChanged(BOOL fSelected)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = S_OK;

	if(fSelected)
	{
		ASSERT(m_pComponent);
		if(m_pComponent == NULL)
		{
			return E_UNEXPECTED;
		}

		ASSERT(m_pComponent->m_pIConductor != NULL);
		if(m_pComponent->m_pIConductor == NULL)
		{
			return E_UNEXPECTED;
		}
		
		// Setup the dummy instrument in case we want to play the wave.
		m_pWave->SetupDummyInstrument();

		SetTransportName();
		UINT uButtonState = BS_PLAY_ENABLED | BS_NO_AUTO_UPDATE;
		if(m_pWave->WaveIsPlaying())
		{
			uButtonState = BS_STOP_ENABLED | BS_NO_AUTO_UPDATE;
		}
		
		m_pComponent->m_pIConductor->SetActiveTransport(this, uButtonState);
	}

	return hr; 
}


HRESULT CWaveNode::SetSelected(BOOL bSelect)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		return E_FAIL;
	}

	IDMUSProdFramework* pIFramework = m_pComponent->m_pIFramework;
	ASSERT(pIFramework);
	if(pIFramework == NULL)
	{
		return E_FAIL;
	}

	IDMUSProdNode* pSelectedNode = NULL;
	if(FAILED(pIFramework->GetSelectedNode(&pSelectedNode)))
	{
		return E_FAIL;
	}
	
	HRESULT hr = E_FAIL;
	if(this != pSelectedNode)
	{
		hr = pIFramework->SetSelectedNode(this);
	}

	pSelectedNode->Release();
	return hr;
}


HRESULT CWaveNode::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		return E_FAIL;
	}

	IDMUSProdFramework* pIFramework = m_pComponent->m_pIFramework;
	ASSERT(pIFramework);
	if(pIFramework == NULL)
	{
		return E_FAIL;
	}

	if(ppIDataObject == NULL)
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

	IStream* pIStream = NULL;
	HRESULT hr = E_FAIL;
	if(SUCCEEDED(pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream)))
	{
		IDMUSProdRIFFStream* pIRiffStream = NULL;
		if(FAILED(AllocRIFFStream(pIStream, &pIRiffStream)))
		{
			pIStream->Release();
			return E_OUTOFMEMORY;
		}
		
		SetDragCopy(false);
		if(FAILED(WriteDragChunkToStream(pIRiffStream)))
		{
			SetDragCopy(false);
			pIRiffStream->Release();
			pIStream->Release();
			return E_FAIL;
		}
		
		SetDragCopy(false);

		// Done with the RIFF stream
		pIRiffStream->Release();

		if(FAILED(hr = pDataObject->AddClipFormat(m_pComponent->m_cfWave, pIStream)))
		{
			pIStream->Release();
			return E_FAIL;
		}

		pIStream->Release();
	}

	if(SUCCEEDED(hr))
	{
		hr = E_FAIL;

		// Create a stream in CF_DMUSPROD_FILE format
		if(SUCCEEDED(m_pComponent->m_pIFramework->SaveClipFormat( m_pComponent->m_cfProducerFile, this, &pIStream ) ) )
		{
			// Container nodes represent files so we must also
			// place CF_DMUSPROD_FILE into CDllJazzDataObject
			hr = pDataObject->AddClipFormat(m_pComponent->m_cfProducerFile, pIStream);
			pIStream->Release();
		}
	}


	if(SUCCEEDED(hr))
	{
		// Already AddRef'd
		*ppIDataObject = pDataObject;	
	}
	else
	{
		pDataObject->Release();
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
//
// CWaveNode IDMUSProdNode::CanCut
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::CanCut()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return S_OK;

}

/////////////////////////////////////////////////////////////////////////////
//
// CWaveNode IDMUSProdNode::CanCopy
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::CanCopy()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return S_OK;

}

/////////////////////////////////////////////////////////////////////////////
//
// CWaveNode IDMUSProdNode::CanDelete
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::CanDelete()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return S_OK;    

}

/////////////////////////////////////////////////////////////////////////////
//
// CWaveNode IDMUSProdNode::CanDeleteChildNode
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;   // no children

}

/////////////////////////////////////////////////////////////////////////////
//
// CWaveNode IDMUSProdNode::CanPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
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

	// Stand alone Wave Nodes are not drop targets
	if(IsInACollection() == false)
	{
		return S_FALSE;
	}


	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = S_FALSE;

	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_pComponent->m_cfWave)))
    {
        hr = S_OK;
    }

    pDataObject->Release();

    return hr;
}

/////////////////////////////////////////////////////////////////////////////
//
// CWaveNode IDMUSProdNode::PasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    // Pass it to the parent to paste.
	if( m_pIRootNode != this )
	{
		if( m_pIParentNode )
		{
			return m_pIParentNode->ChildPasteFromData( pIDataObject, this );
		}
	}

	return E_FAIL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CWaveNode IDMUSProdNode::CanChildPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
									  BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;   // no children

}

/////////////////////////////////////////////////////////////////////////////
//
// CWaveNode IDMUSProdNode::ChildPasteFromData
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;   // no children

}

/////////////////////////////////////////////////////////////////////////////
// CWaveNode IDMUSProdNode::GetObject

HRESULT CWaveNode::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(::IsEqualCLSID(rclsid, CLSID_DirectSoundWave))
	{
		if(m_pIDirectSoundWave)
		{
			return m_pIDirectSoundWave->QueryInterface(riid, ppvObject);
		}
	}


	return E_NOTIMPL;
}

////////////////////////////////////////////////////////////////////////////////
//
// IDMUSProdnotifySink Implementation
//
////////////////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::OnUpdate(IDMUSProdNode* pIDocRootNode, GUID guidUpdateType, VOID* pData)
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_FAIL;
	}

	if(pIDocRootNode != m_pIRootNode)
	{
		return E_NOTIMPL;
	}

	static bool bActualNameChange = false;

	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(::IsEqualGUID( guidUpdateType, FRAMEWORK_BeforeFileSave))
	{
		ASSERT(pData);
		if(pData == NULL)
		{
			return E_INVALIDARG;
		}

		WCHAR* pwszFileName = (WCHAR*)pData;
		CString sFileName = pwszFileName;

		// Rename the file if we're the docroot
		if(IsInACollection() == false)
		{
			// We're not getting saved are we?
			if(m_sFileName != sFileName)
			{
				return S_OK;
			}

			// Get a temp file name
			char szTempPath[MAX_PATH];
			DWORD dwSuccess = GetTempPath(MAX_PATH, szTempPath);
			CString sTempPath = szTempPath;
			
			char szTempName[MAX_PATH];
			CString sTempName;
			
			CString sPrefix = "DMP";
			CString sExtension = "tmp";
			if(FAILED(CWaveDataManager::CreateUniqueTempFile(sPrefix, sExtension, sTempName)))
			{
				GetTempFileName(sTempPath, sPrefix, 0, szTempName);
				sTempName = szTempName;
			}

			// Delete the created file otherwise the rename will fail 
			DeleteFile(sTempName);

			EnterCriticalSection(&m_CriticalSection);
			if(FAILED(m_pWave->PrepareForSave(sTempName)))
			{
				LeaveCriticalSection(&m_CriticalSection);
				return E_FAIL;
			}

			LeaveCriticalSection(&m_CriticalSection);
		}
		return S_OK;
	}

	if(::IsEqualGUID(guidUpdateType, FRAMEWORK_AfterFileSave))
	{
		ASSERT(pData);
		if(pData == NULL)
		{
			return E_INVALIDARG;
		}

		WCHAR* pwszFileName = (WCHAR*)pData;
		CString sFileName = pwszFileName;

		if(IsInACollection() == false)
		{
			// We're not getting saved are we?
			if(m_sFileName != sFileName)
			{
				return S_OK;
			}

			EnterCriticalSection(&m_CriticalSection);
			if(FAILED(m_pWave->CleanupAfterSave(sFileName)))
			{
				LeaveCriticalSection(&m_CriticalSection);
				return E_FAIL;
			}

			LeaveCriticalSection(&m_CriticalSection);
		}

		return S_OK;
	}

	if(::IsEqualGUID( guidUpdateType, FRAMEWORK_BeforeFileNameChange))
	{
		// We need to close out all the open file handles
		bActualNameChange = true;
		EnterCriticalSection(&m_CriticalSection);
		if(FAILED(m_pWave->CloseSourceHandler()))
		{
			LeaveCriticalSection(&m_CriticalSection);
			bActualNameChange = false;
			return E_FAIL;
		}

		return S_OK;
	}

	if(::IsEqualGUID( guidUpdateType, FRAMEWORK_AbortFileNameChange))
	{
		bActualNameChange = false;
		if(FAILED(m_pWave->OnSourceRenamed(m_sFileName)))
		{
			LeaveCriticalSection(&m_CriticalSection);
			return E_FAIL;
		}

		LeaveCriticalSection(&m_CriticalSection);

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
            bActualNameChange = false;
			LeaveCriticalSection(&m_CriticalSection);
			return E_FAIL;
		}

		CString sFileName = bstrNodeFileName;
		::SysFreeString(bstrNodeFileName);

		// Are we already looking at the same file?
		if(m_sFileName == sFileName)
		{
            bActualNameChange = false;
			return S_OK;
		}

		// Keep the name with the wave node...
		SetFileName(sFileName);

		if(!bActualNameChange)
		{
			EnterCriticalSection(&m_CriticalSection);
		}

		if(FAILED(m_pWave->FileNameChanged(sFileName)))
		{
            bActualNameChange = false;
			LeaveCriticalSection(&m_CriticalSection);
			return E_FAIL;
		}

		LeaveCriticalSection(&m_CriticalSection);
		m_pWave->NotifyWaveChange(false);
        bActualNameChange = false;

		return S_OK;
	}

	if(::IsEqualGUID(guidUpdateType, FRAMEWORK_AfterFileOpen))
	{
        bActualNameChange = false;

		// Check if this is during a drop-paste
		if(m_bPastingWave)
		{
			m_bPastingWave = false;
			DeleteFile(m_sFileName);

			// We need to close out all the open file handles
			bActualNameChange = true;
			EnterCriticalSection(&m_CriticalSection);
			if(FAILED(m_pWave->CloseSourceHandler()))
			{
                bActualNameChange = false;
				LeaveCriticalSection(&m_CriticalSection);
				return E_FAIL;
			}

			try
			{
				CFile::Rename(m_sTempFileName, m_sFileName);
			}
			catch(CFileException e)
			{
                bActualNameChange = false;
				LeaveCriticalSection(&m_CriticalSection);
				return E_FAIL;
			}

			if(FAILED(m_pWave->FileNameChanged(m_sFileName)))
			{
                bActualNameChange = false;
				LeaveCriticalSection(&m_CriticalSection);
				return E_FAIL;
			}

			LeaveCriticalSection(&m_CriticalSection);
            bActualNameChange = false;

			return S_OK;

		}	
	}

	return E_NOTIMPL;
}



//////////////////////////////////////////////////////////////////////
//
//	CWaveNode IPersist::GetClassID
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::GetClassID(CLSID* pClsId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT(pClsId != NULL);

    memset(pClsId, 0, sizeof( CLSID ));

    return S_OK;
}

//////////////////////////////////////////////////////////////////////
// CWaveNode : IPersistStream Implementation
// This implementation is just a wrapper and basically calls the 
// methods implemented in the contained CWave object

//////////////////////////////////////////////////////////////////////
//
// CWaveNode IPersistStream::IsDirty
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::IsDirty()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return m_bIsDirty ? S_OK : S_FALSE;
}

//////////////////////////////////////////////////////////////////////
//
// CWaveNode IPersistStream::GetSizeMax
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::GetSizeMax( ULARGE_INTEGER FAR* /*pcbSize*/ )
{
    // I want to know if I am called
	ASSERT(FALSE);

	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////
//
// CWaveNode IPersistStream::Load
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::Load(IStream* pIStream)
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		return E_FAIL;
	}

	ASSERT(m_pComponent->m_pIFramework);
	if(m_pComponent->m_pIFramework == NULL)
	{
		return E_FAIL;
	}

	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_POINTER;
	}
	
	HRESULT hr = E_FAIL;
	MMCKINFO ckMain;
    IDMUSProdRIFFStream* pRiffStream = NULL;

	if(SUCCEEDED(hr = AllocRIFFStream(pIStream, &pRiffStream)))
	{
		DWORD dwPos = StreamTell(pIStream);
		hr = E_FAIL;
		ckMain.fccType = mmioFOURCC('W','A','V','E');
		while(pRiffStream->Descend(&ckMain, NULL, MMIO_FINDRIFF) == 0 )
		{
			if(FAILED(hr = Load(pRiffStream, &ckMain)))
			{
				pRiffStream->Release();
				return E_FAIL;
			}
			pRiffStream->Ascend(&ckMain, 0);
		}

		// We don't have any actual waves persisted in the stream
		// This might then be the drag chunk you think?
		if(FAILED(hr))
		{
			StreamSeek(pIStream, dwPos, STREAM_SEEK_SET);
			ckMain.ckid = FOURCC_DRAG;
			ckMain.fccType = 0;

			if(pRiffStream->Descend(&ckMain, NULL, MMIO_FINDCHUNK) == 0 )
			{
				if(FAILED(hr = PasteWave(pRiffStream, &ckMain)))
				{
					pRiffStream->Release();
					return E_FAIL;
				}

				pRiffStream->Ascend(&ckMain, 0);
			}
		}

		pRiffStream->Release();
	}


	if(SUCCEEDED(hr) && m_pIDirectSoundWave == NULL && m_bInCollection == false)
	{
		CreateDirectSoundWave();
	}

	ClearDirtyFlag();

	return hr;
}

HRESULT CWaveNode::PasteWave(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckDragged)
{
	ASSERT(pIRiffStream);
	if(pIRiffStream == NULL)
	{
		return E_POINTER;
	}

	ASSERT(pckDragged);
	if(pckDragged == NULL)
	{
		return E_POINTER;
	}

	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		return E_FAIL;
	}
	
	IDMUSProdFramework* pIFramework = m_pComponent->m_pIFramework;
	ASSERT(pIFramework);
	if(pIFramework == NULL)
	{
		return E_FAIL;
	}

	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_FAIL;
	}

	// Get the node guid saved in the chunk
	GUID guidFile;
	DWORD cb = 0;
	HRESULT hr = pIStream->Read(&guidFile, sizeof(GUID), &cb);
	if(FAILED(hr) || cb != sizeof(GUID))
	{
		pIStream->Release();
		return E_FAIL;
	}


	GUID guidNode;
	cb = 0;
	hr = pIStream->Read(&guidNode, sizeof(GUID), &cb);
	if(FAILED(hr) || cb != sizeof(GUID))
	{
		pIStream->Release();
		return E_FAIL;
	}

	// Done with the stream
	pIStream->Release();
	pIRiffStream->Ascend(pckDragged, 0);

	// Find the wave node that owns this wave file
	IDMUSProdNode* pIDocRootNode = NULL;
	if(FAILED(pIFramework->FindDocRootNodeByFileGUID(guidFile, &pIDocRootNode)))
	{
		return E_FAIL;
	}

	// Get the file name for the node we're copying from
	BSTR bstrSourceFileName;
	if(FAILED(pIFramework->GetNodeFileName(pIDocRootNode, &bstrSourceFileName)))
	{
		pIDocRootNode->Release();
		return E_FAIL;
	}

	CString sSourceFileName = bstrSourceFileName;
	SysFreeString(bstrSourceFileName);

	// Get a temp file name to copy the source into
	char szTempPath[MAX_PATH];
	GetTempPath(MAX_PATH, szTempPath);
	char szTempFileName[MAX_PATH];

	CString sPrefix = "DMP";
	CString sExtension = "tmp";
	if(FAILED(CWaveDataManager::CreateUniqueTempFile(sPrefix, sExtension, m_sTempFileName)))
	{
		GetTempFileName(szTempPath, sPrefix, 0, szTempFileName);
		m_sTempFileName = szTempFileName;
	}

    m_bSrcInATempFile = TRUE;

	IStream* pIFileStream = NULL;
	if(FAILED(AllocStreamToFile(&pIFileStream, m_sTempFileName, 0)))
	{
		return E_FAIL;
	}

	CWaveNode* pSourceWaveNode = dynamic_cast<CWaveNode*>(pIDocRootNode);

	// This could be a wave in the collection
	if(pSourceWaveNode == NULL)
	{
		CCollection* pCollectionNode = dynamic_cast<CCollection*>(pIDocRootNode);
		ASSERT(pCollectionNode);
		if(pCollectionNode == NULL)
		{
			pIFileStream->Release();
			return E_FAIL;
		}

		if(FAILED(pCollectionNode->FindWaveNodeByGUID(guidNode, &pSourceWaveNode)))
		{
			pIFileStream->Release();
			return E_FAIL;
		}
	}

	ASSERT(pSourceWaveNode);
	if(pSourceWaveNode == NULL)
	{
		pIFileStream->Release();
		return E_FAIL;
	}

    // Ask the source wave to update it's uncompressed deltas
	if(FAILED(pSourceWaveNode->SaveAs(pIFileStream, false)))
	{
		pIFileStream->Release();
		return E_FAIL;
	}

	pIDocRootNode->Release();
	pIFileStream->Release();

	CString sOldName = m_sFileName;
	SetFileName(m_sTempFileName);

	// Now get a read stream from this file and load the wave
	if(FAILED(AllocStreamFromFile(&pIFileStream, m_sTempFileName, 0)))
	{
		DeleteFile(m_sTempFileName);
		m_sTempFileName = "";
		return E_FAIL;
	}

	if(FAILED(Load(pIFileStream)))
	{
		pIFileStream->Release();
		return E_FAIL;
	}

	if(IsInACollection())
	{
		SetPreviousRegionLinkIDs(pSourceWaveNode);
	}

	// Done with this stream
	pIFileStream->Release();

	// Remember that we're pasting waves
	m_bPastingWave = true;

	// Reset the correct name
	SetFileName(sOldName);

	return S_OK;
}


BOOL CWaveNode::IsSourceInTempFile()
{
    return m_bSrcInATempFile;
}

void CWaveNode::SetSourceInTempFile(BOOL bSourceInTempFile)
{
    m_bSrcInATempFile = bSourceInTempFile;
}


HRESULT CWaveNode::Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain)
{
	ASSERT(pIRiffStream);
	if(pIRiffStream == NULL)
		return E_POINTER;

	bool bLoadingStereo = false;
	bool bTrueStereo = false;
	bool bRightChannel = false;
	GUID guidStereoWave;
	::CoCreateGuid(&guidStereoWave);
	
	GUID guidTemp;
	CopyGUID(guidStereoWave, guidTemp);
	
	if(SUCCEEDED(IsLoadingStereoWave(pIRiffStream, pckMain, bLoadingStereo, bTrueStereo, guidStereoWave, bRightChannel)))
	{
		if(bLoadingStereo)
		{
			// If there's no GUID saved we know that it's a brand new TRUE Stereo Wave
			//BOOL bTrueStereo = ::IsEqualGUID(guidTemp, guidStereoWave);

			if(m_pWave && m_bStereo == false)
			{
				return E_FAIL;
			}
			
			if(m_pWave == NULL)
			{
				m_pWave = new CStereoWave(this, bTrueStereo);
				m_bStereo = true;
			}

			if(bTrueStereo)
			{
				if(FAILED(m_pWave->Load(pIRiffStream, pckMain)))
				{
					delete m_pWave;
					m_pWave = NULL;
					return E_FAIL;
				}
			}
			else
			{
				((CStereoWave*)m_pWave)->SetGUID(guidStereoWave);
				if(FAILED(((CStereoWave*)m_pWave)->Load(pIRiffStream, pckMain, bRightChannel)))
				{
					delete m_pWave;
					m_pWave = NULL;
					return E_FAIL;
				}
			}
		}
		else
		{
			if(m_pWave && m_bStereo == true)
			{
				return E_FAIL;
			}
			else if(m_pWave == NULL)
			{
				m_pWave = new CMonoWave(this);
				m_bStereo = false;
			}

			if(FAILED(m_pWave->Load(pIRiffStream, pckMain)))
			{
				delete m_pWave;
				m_pWave = NULL;
				return E_FAIL;
			}

		}
	}
	else
	{
		return E_FAIL;
	}

	ClearDirtyFlag();

	return S_OK;
}


///////////////////////////////////////////////////////////////////////////
//
// Creates a DirectSoundWave object
//
//////////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::CreateDirectSoundWave()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	HRESULT hr = E_FAIL;

	// Create a DirectSoundWave object only when NOT in a collection
	if(m_pIDirectSoundWave == NULL && m_bInCollection == false)
	{
		if(SUCCEEDED(hr = ::CoCreateInstance(CLSID_DirectSoundWave, NULL, CLSCTX_INPROC, IID_IDirectSoundWave, (void**)&m_pIDirectSoundWave)))
		{
			IDirectMusicObject* pIDirectMusicObject = NULL;
			if(SUCCEEDED(hr = m_pIDirectSoundWave->QueryInterface(IID_IDirectMusicObject, (void**)&pIDirectMusicObject)))
			{
				if(m_pDirectSoundWaveStream == NULL)
				{
					m_pDirectSoundWaveStream = new CWaveStream(m_pWave);
				}

				if(m_pDirectSoundWaveStream)
				{
					// Persist the wave into the stream
					DMUS_OBJECTDESC objDesc;
					ZeroMemory(&objDesc, sizeof(DMUS_OBJECTDESC));

					objDesc.dwSize = sizeof(DMUS_OBJECTDESC);
					objDesc.dwValidData = DMUS_OBJ_STREAM;
					objDesc.pStream = m_pDirectSoundWaveStream;

					pIDirectMusicObject->SetDescriptor(&objDesc);

					IPersistStream* pIPersistStream;
					if(SUCCEEDED(hr = m_pIDirectSoundWave->QueryInterface(IID_IPersistStream, (void **)&pIPersistStream )))
					{
						StreamSeek(m_pDirectSoundWaveStream, 0, STREAM_SEEK_SET);
						hr = pIPersistStream->Load(m_pDirectSoundWaveStream);

						RELEASE(pIPersistStream);
					}
				}

				pIDirectMusicObject->Release();
			}
		}
	}

	// Cleanup if we failed
	if(FAILED(hr))
	{
		if(m_pDirectSoundWaveStream)
		{
			m_pDirectSoundWaveStream->Release();
			m_pDirectSoundWaveStream= NULL;
		}
		if(m_pIDirectSoundWave)
		{
			m_pIDirectSoundWave->Release();
			m_pIDirectSoundWave = NULL;
		}
	}

	return hr;
}


HRESULT	CWaveNode::ReloadDirectSoundWave()
{
	HRESULT hr = E_FAIL;
	if(m_pIDirectSoundWave)
	{
		// Do a reload on the wave
		IPersistStream* pIPersistStream = NULL;
		if(SUCCEEDED(hr = m_pIDirectSoundWave->QueryInterface(IID_IPersistStream, (void **)&pIPersistStream)))
		{
			StreamSeek(m_pDirectSoundWaveStream, 0, STREAM_SEEK_SET);
			hr = pIPersistStream->Load(m_pDirectSoundWaveStream);

			RELEASE(pIPersistStream);
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
//
// CWaveNode IPersistStream::Save
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveNode::Save(IStream* pIStream, BOOL fClearDirty)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT(pIStream);
	if(pIStream == NULL)
		return E_POINTER;

	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return E_FAIL;

	return m_pWave->Save(pIStream, fClearDirty);
}

HRESULT CWaveNode::SaveAs(IStream* pIStream, bool bPromptForFileName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT(pIStream);
	if(pIStream == NULL)
		return E_POINTER;

	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return E_FAIL;

	return m_pWave->SaveAs(pIStream, bPromptForFileName);
}


// =================================================================================
//
// IDMUSProdTransport methods
//
// =================================================================================


UINT AFX_CDECL WaveStopResetFunction(LPVOID pParam)
{
	WaveStopResetTimeEvent* resetTimeEvent = (WaveStopResetTimeEvent*) pParam;
	WaitForSingleObject(resetTimeEvent->pWaveNode->m_hStopEvent, resetTimeEvent->lWaitTime);

	resetTimeEvent->pWaveNode->Stop( TRUE );
	TRACE("Killing update thread\n");
	::AfxEndThread(0);

	return 0;
}


HRESULT CWaveNode::Play(BOOL /*fPlayFromStart*/)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ResetEvent(m_hStopEvent);

	// Return failure if we don't have a valid port to play the wave
	if(m_pComponent->m_lstDLSPortDownload.IsEmpty())
	{
		AfxMessageBox(IDS_ERR_NO_PORT, MB_OK | MB_ICONEXCLAMATION);
		return E_FAIL;
	}

	if(m_pWave)
	{
		if(SUCCEEDED(m_pWave->Play(TRUE)))
		{
			// Setup a timer so we can keep track of playback
			if (m_pWaveCtrl && m_bTrackTimeCursor ) 
			{
				// tell the wave control playing has begun
				m_pWaveCtrl->PreparePlaybackTimer();
			}
			else if(m_pWave->IsLooped() == false)
			{
				m_ResetCallBackParams.lWaitTime = (long) ((DWORD)1000*((float)m_pWave->GetWaveLength() / m_pWave->m_rWaveformat.nSamplesPerSec));
				m_ResetCallBackParams.pWaveNode = this;

				CWinThread* pUpdateThread = ::AfxBeginThread(WaveStopResetFunction, &m_ResetCallBackParams);
				pUpdateThread->m_bAutoDelete = TRUE;

			}
					
			m_pComponent->m_pIConductor->SetBtnStates(this, BS_PLAY_CHECKED | BS_STOP_ENABLED  | BS_NO_AUTO_UPDATE);


			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CWaveNode::Stop( BOOL /*fStopImmediate*/ )
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_FAIL;
	}

	SetEvent(m_hStopEvent);

	if (m_pWaveCtrl) 
	{
		m_pWaveCtrl->KillPlaybackTimer();
	}

	m_pWave->TurnOffMidiNotes();

    ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		return E_UNEXPECTED;
	}

	// Register with the transport
	IDMUSProdConductor* pIConductor = m_pComponent->m_pIConductor;
	ASSERT(pIConductor);
	if(pIConductor)
	{
        pIConductor->TransportStopped(this);
    }

    return S_OK;
}

HRESULT CWaveNode::Transition ( )
{
    return E_NOTIMPL;
}

HRESULT CWaveNode::Record( BOOL fPressed )
{
	return E_NOTIMPL;
}

HRESULT CWaveNode::GetName(BSTR* pbstrName)
{
	if(m_pWave == NULL)
		return E_FAIL;
	
	*pbstrName = m_pWave->GetName().AllocSysString();
	return S_OK;
}

HRESULT CWaveNode::TrackCursor( BOOL fTrackCursor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_bTrackTimeCursor = (fTrackCursor != 0);

	if( m_pWaveCtrl && m_pWave->WaveIsPlaying())
	{
		if( fTrackCursor )
		{
			// tell the wave control to start the cursor
			m_pWaveCtrl->PreparePlaybackTimer();
		}
		else
		{
			// tell the wave control to stop the cursor
			m_pWaveCtrl->KillPlaybackTimer();
		}
	}

	return S_OK;
}

//==================================================================
//
//	IDMUSProdWaveTimelineDraw methods
//
//==================================================================
HRESULT CWaveNode::DrawWave(IDMUSProdTimeline* pITimeline, HDC hDC, const CRect& rcClient, const long lScrollOffset, const WaveDrawParams* pWaveDrawParams, const WaveTimelineInfoParams* pWaveInfoParams)
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_FAIL;
	}

	ASSERT(pITimeline);
	if(pITimeline == NULL)
	{
		return E_POINTER;
	}

	// Can't draw on a 0 width rect?
	ASSERT(rcClient.Width());
	if(rcClient.Width() <= 0)
	{
		return E_INVALIDARG;
	}


	ASSERT(pWaveDrawParams);
	if(pWaveDrawParams == NULL)
	{
		return E_POINTER;
	}

	ASSERT(pWaveInfoParams);
	if(pWaveInfoParams == NULL)
	{
		return E_POINTER;
	}

	if(pWaveDrawParams->cbSize < sizeof(WaveDrawParams))
	{
		return E_INVALIDARG;
	}

	if(pWaveInfoParams->cbSize < sizeof(WaveTimelineInfoParams))
	{
		return E_INVALIDARG;
	}

	// Collect the list of tempo changes between start time and end time
	CPtrList lstTempoChanges;
	DWORD dwGroupBits = pWaveInfoParams->dwGroupBits;
	REFERENCE_TIME rtOffset = pWaveInfoParams->rtOffset;
	REFERENCE_TIME rtStart = pWaveInfoParams->rtStart + rtOffset;
	REFERENCE_TIME rtEnd = rtStart + pWaveInfoParams->rtDuration;
	
	// Get the start and end in MUSIC_TIME units
	MUSIC_TIME mtStart = 0;
	if(FAILED(pITimeline->RefTimeToClocks(pWaveInfoParams->rtStart, &mtStart)))
	{
		return E_FAIL;
	}

	MUSIC_TIME mtEnd = 0;
	if(FAILED(pITimeline->RefTimeToClocks(rtEnd, &mtEnd)))
	{
		return E_FAIL;
	}

	// Get a CDC object from the hDC
	CDC dcClient;
	if(dcClient.Attach(hDC) == 0)
	{
		return E_FAIL;
	}

	// Collect the tempos now
	while(mtStart < mtEnd)
	{
		HRESULT hr = E_FAIL;
		DMUS_TEMPO_PARAM tempo;
		MUSIC_TIME mtNext = 0;
		if(SUCCEEDED(hr = pITimeline->GetParam(GUID_TempoParam, dwGroupBits, 0, mtStart, &mtNext, &tempo)))
		{
			REFERENCE_TIME rtTempoStart = 0;
			REFERENCE_TIME rtTempoEnd = 0;
			if(SUCCEEDED(hr = pITimeline->ClocksToRefTime(tempo.mtTime, &rtTempoStart)))
			{
				mtNext += mtStart;
				if(SUCCEEDED(hr = pITimeline->ClocksToRefTime((mtNext - 1), &rtTempoEnd)))
				{
					TempoChange* pTempoChange = new TempoChange;
					pTempoChange->rtTempoStart = rtTempoStart;
					pTempoChange->rtTempoEnd = rtTempoEnd;
					
					// The next tempo could be 0 in case of a default tempo or it wraps around
					if(mtNext == 0 || mtNext <= mtStart || rtTempoEnd > rtEnd)
					{
						if(pTempoChange->rtTempoStart < pWaveInfoParams->rtStart)
						{
							pTempoChange->rtTempoStart = pWaveInfoParams->rtStart;
						}

						pTempoChange->rtTempoEnd = rtEnd;
					}
					pTempoChange->dblTempo = tempo.dblTempo;
					lstTempoChanges.AddTail(pTempoChange);
					
					if(mtNext == 0 || mtNext <= mtStart)
					{
						break;
					}

					mtStart = mtNext;
				}
			}
		}

		// Delete the list if we failed
		if(FAILED(hr))
		{
			while(!lstTempoChanges.IsEmpty())
			{
				delete lstTempoChanges.RemoveHead();
				dcClient.Detach();
			}
			return E_FAIL;
		}
	}

	
	WAVEFORMATEX waveFormat = m_pWave->m_rWaveformat;
	DWORD dwSampleRate = waveFormat.nSamplesPerSec;
	if(FAILED(GetFineTunedSampleRate(pWaveInfoParams->lPitch, dwSampleRate)))
	{
		return E_FAIL;
	}

	DWORD dwWaveLength = m_pWave->GetWaveLength();

	if(rtOffset > 0)
	{
		// Draw the clipped boundaries for the wave...if we're clipped that is
		COLORREF clrClip = pWaveDrawParams->clrClipMarker;
		CPen clipMarker(PS_DOT, 0, clrClip);
		CPen* pOldPen = dcClient.SelectObject(&clipMarker);
		dcClient.MoveTo(rcClient.left, rcClient.top);
		dcClient.LineTo(rcClient.left, rcClient.bottom);
		dcClient.SelectObject(pOldPen);
	}
	else
	{
		// Mark the start of the wave
		COLORREF clrStart = pWaveDrawParams->clrStartMarker;
		CPen startPen(PS_SOLID, 0, clrStart);
		CPen* pOldPen = dcClient.SelectObject(&startPen);
		dcClient.MoveTo(rcClient.left, rcClient.top);
		dcClient.LineTo(rcClient.left, rcClient.bottom);
		dcClient.SelectObject(pOldPen);
	}

	// The first sample is at the offset from the start
	DWORD dwLastSampleDrawn = (DWORD)((((double)(rtOffset)/10000)/1000) * dwSampleRate);

	DWORD dwPlayLength = (DWORD)((((double)(pWaveInfoParams->rtDuration) / 10000) / 1000) * dwSampleRate);

	// The end sample is at rtDuration from the start sample
	DWORD dwEndSample = dwLastSampleDrawn + dwPlayLength;
	if(dwEndSample < (dwWaveLength - 1))
	{
		// Draw the clipped boundaries for the wave...if we're clipped that is
		COLORREF clrClip = pWaveDrawParams->clrClipMarker;
		CPen clipMarker(PS_DOT, 0, clrClip);
		CPen* pOldPen = dcClient.SelectObject(&clipMarker);
		dcClient.MoveTo(rcClient.right, rcClient.top);
		dcClient.LineTo(rcClient.right, rcClient.bottom);
		dcClient.SelectObject(pOldPen);
	}
	else
	{
		// Mark the end of the wave
		COLORREF clrEnd = pWaveDrawParams->clrEndMarker;
		CPen endPen(PS_SOLID, 0, clrEnd);
		CPen* pOldPen = dcClient.SelectObject(&endPen);
		dcClient.MoveTo(rcClient.right, rcClient.top);
		dcClient.LineTo(rcClient.right, rcClient.bottom);
		dcClient.SelectObject(pOldPen);
	}

	// Collect the error in every draw call..these are the samples not drawn due to 
	// the precision error in fSamplesPerPixel
	int nError = 0;

	// get clip rectangle
	CRect rcClip;
	::GetClipBox(hDC, &rcClip);
	REFERENCE_TIME rtClip;

	POSITION position = lstTempoChanges.GetHeadPosition();
	while(position)
	{
		TempoChange* pTempoChange = (TempoChange*) lstTempoChanges.GetNext(position);
		ASSERT(pTempoChange);
		if(pTempoChange)
		{
			// Get the samples per beat
			double dblBeatsPerSecond = pTempoChange->dblTempo / 60;
			double dblSamplesPerBeat = dwSampleRate / dblBeatsPerSecond;

			REFERENCE_TIME rtRectStart = pWaveInfoParams->rtStart;

			// Calculate how many samples we have to draw at this tempo 
			REFERENCE_TIME rtDrawStart = rtStart;
			rtDrawStart = (pTempoChange->rtTempoStart + rtOffset) > rtDrawStart ? (pTempoChange->rtTempoStart + rtOffset) : rtStart;
			rtRectStart = pTempoChange->rtTempoStart > rtRectStart ? pTempoChange->rtTempoStart : rtRectStart;
			// note that we could clip rtDrawStart to rcClip.left for speed... but then we'd have to change dwLastSampleDrawn too

			// end of draw
			REFERENCE_TIME rtDrawEnd;
			rtDrawEnd = rtEnd > (pTempoChange->rtTempoEnd + rtOffset) ? (pTempoChange->rtTempoEnd + rtOffset) : rtEnd;
			rtClip = 0;
			if (SUCCEEDED(pITimeline->PositionToRefTime(rcClip.right - (-lScrollOffset+1), &rtClip)))
				if (rtDrawEnd > rtClip+rtOffset)
					rtDrawEnd = rtClip+rtOffset;

			// Drawing duration in seconds
			double dblDrawDuration = ((double)(rtDrawEnd - rtDrawStart) / 10000) / 1000; 

			// Number of samples that are in this drawing duration
			double dblBeatsInChange = dblDrawDuration * dblBeatsPerSecond;
			double dblSamplesInChange = dblBeatsInChange * dblSamplesPerBeat;
			DWORD dwSamplesInChange = (DWORD)(dblSamplesInChange) + nError;
			
			// Get the samples per pixel and the clipped rect for drawing the wave
			CRect rcDraw = rcClient;
			long lStartPos = 0;
			if(SUCCEEDED(pITimeline->RefTimeToPosition(rtRectStart, &lStartPos)))
			{
				long lEndPos = 0;
				if(SUCCEEDED(pITimeline->RefTimeToPosition((rtDrawEnd - rtOffset), &lEndPos)))
				{
					lStartPos += -lScrollOffset + 1;
					lEndPos += -lScrollOffset + 1;
					rcDraw.left = lStartPos <= rcClient.left ? rcClient.left : lStartPos;
					rcDraw.right = lEndPos >= rcClient.right ? rcClient.right : lEndPos;
				}
			}

			//  Actually sample units per pixel
			float fSamplesPerPixel =  (((float)dwSamplesInChange / DRAW_UNIT_SIZE) / rcDraw.Width());

			//  Determine offset into wave (first sample to draw)
			if(	pWaveInfoParams->dwFlags & WAVE_LOOPED
			&&  dwLastSampleDrawn >= dwWaveLength )
			{
				DWORD dwLoopLength = pWaveInfoParams->dwLoopEnd - pWaveInfoParams->dwLoopStart;
				if( dwLoopLength )
				{
					dwLastSampleDrawn -= pWaveInfoParams->dwLoopStart;
					dwLastSampleDrawn  = dwLastSampleDrawn % dwLoopLength;
					dwLastSampleDrawn += pWaveInfoParams->dwLoopStart;
				}
			}

			DWORD dwTemp = dwLastSampleDrawn;
			
			EnterCriticalSection(&m_CriticalSection);
			m_pWave->OnDraw(&dcClient, rcDraw, lScrollOffset, dwLastSampleDrawn, dwSamplesInChange, fSamplesPerPixel, pWaveDrawParams, pWaveInfoParams);
			LeaveCriticalSection(&m_CriticalSection);

			// Update the error for the next draw
			dwLastSampleDrawn = dwTemp + dwSamplesInChange;
			//nError = (dwTemp + dwSamplesInChange) - dwLastSampleDrawn;
		}
	}

	// Detach the hDC
	dcClient.Detach();

	// Delete the tempo change list
	while(!lstTempoChanges.IsEmpty())
	{
		delete lstTempoChanges.RemoveHead();
	}

	return S_OK;	
}

HRESULT CWaveNode::GetWaveInfo(WaveInfoParams* pWaveInfoParams)
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_FAIL;
	}

	ASSERT(pWaveInfoParams);
	if(pWaveInfoParams == NULL)
	{
		return E_POINTER;
	}

	ASSERT(pWaveInfoParams->cbSize == sizeof(WaveInfoParams));
	if(pWaveInfoParams->cbSize != sizeof(WaveInfoParams))
	{
		return E_INVALIDARG;
	}
	
	pWaveInfoParams->dwWaveDuration = m_pWave->GetWaveLength();

	WAVEFORMATEX waveFormat = m_pWave->m_rWaveformat;

	// Get the duration in REFERENCE_TIME
	double dblWaveLength = (double) m_pWave->GetWaveLength();
	double dblWaveDuration = dblWaveLength / waveFormat.nSamplesPerSec;
	REFERENCE_TIME rtDuration = (__int64)((dblWaveDuration * 10000) * 1000);		// Converts to 100 nanoseconds intervals

	// Set the loop information;
	pWaveInfoParams->dwFlags = 0;
	if(m_pWave->IsLooped())
	{
		pWaveInfoParams->dwFlags |= WAVE_LOOPED;
	}

	RLOOP rloop = m_pWave->GetRLOOP();
	pWaveInfoParams->dwLoopType = rloop.dwType;
	
	pWaveInfoParams->dwLoopStart = rloop.dwStart;
	pWaveInfoParams->dwLoopEnd = rloop.dwEnd;

	if(m_pWave->IsStreamingWave())
	{
		pWaveInfoParams->dwFlags |= WAVE_STREAMING;
	}
	
	double dblLoopStartTime = ((double)rloop.dwStart / waveFormat.nSamplesPerSec);
	double dblLoopEndTime = ((double)rloop.dwEnd / waveFormat.nSamplesPerSec);

	pWaveInfoParams->guidVersion = m_guidVersion;

	return S_OK;
}


HRESULT CWaveNode::SampleToRefTime(DWORD dwSample, REFERENCE_TIME* prtSampleTime, LONG lFineTuneCents)
{
	ASSERT(prtSampleTime);
	if(prtSampleTime == NULL)
	{
		return E_POINTER;
	}

	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_FAIL;
	}

	if(dwSample > m_pWave->GetWaveLength())
	{
		return E_FAIL;
	}
	

	WAVEFORMATEX waveFormat = m_pWave->m_rWaveformat;
	DWORD dwTunedSampleRate = waveFormat.nSamplesPerSec;
	if(FAILED(GetFineTunedSampleRate(lFineTuneCents, dwTunedSampleRate)))
	{
		return E_FAIL;
	}

	double dblSampleTime =  ((double)dwSample / dwTunedSampleRate);
	*prtSampleTime = (__int64)(((dblSampleTime * 10000) + 0.5) * 1000);

	return S_OK;
}


HRESULT CWaveNode::RefTimeToSample(REFERENCE_TIME rtSampleTime, DWORD* pdwSample, LONG lFineTuneCents)
{
	ASSERT(pdwSample);
	if(pdwSample == NULL)
	{
		return E_POINTER;
	}

	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_FAIL;
	}

	WAVEFORMATEX waveFormat = m_pWave->m_rWaveformat;
	DWORD dwTunedSampleRate = waveFormat.nSamplesPerSec;
	if(FAILED(GetFineTunedSampleRate(lFineTuneCents, dwTunedSampleRate)))
	{
		return E_FAIL;
	}

	double dblSampleTime = ((double)rtSampleTime / 10000) / 1000;
	*pdwSample = (DWORD)((dblSampleTime * dwTunedSampleRate) + 0.5);

	return S_OK;
}


HRESULT CWaveNode::GetFineTunedSampleRate(long lFineTuneCents, DWORD& dwTunedSampleRate)
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_UNEXPECTED;
	}

	WAVEFORMATEX waveFormat = m_pWave->m_rWaveformat;

	// 1.059463094359 is 12th root of 2; 0.9438743126817 is 1/12th root of 2
	// If the pitch is shifted up then we have to divide by 12th root of 2
	// If the pitch is shifted down then we have to divide by 12th root of 2

	bool bPositiveTune = lFineTuneCents > 0 ? true : false;
	float fTimeShiftFactor = (float)(bPositiveTune ? 1.0005777895065548592967925757932 : 0.99942254414138074953252875691576);

	double dblTunedSampleRate = waveFormat.nSamplesPerSec;
	for(long lTuneCent = 0; lTuneCent < abs(lFineTuneCents); lTuneCent++)
	{
		dblTunedSampleRate *= fTimeShiftFactor;
	}

    dwTunedSampleRate = (DWORD)(dblTunedSampleRate + 0.5);

	return S_OK;
}


//==================================================================
//
//	Helper Methods
//
//==================================================================

CWave* CWaveNode::GetWave() const 
{
	return m_pWave;
}


CDLSComponent* CWaveNode::GetComponent() const 
{
	// We should always have a valid component right?
	ASSERT(m_pComponent);

	if(m_pComponent)
	{
		return m_pComponent;
	}

	return NULL;
}

void CWaveNode::SetCollection(CCollection* pCollection)
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	m_bInCollection = false;
	if(pCollection)
	{
		m_bInCollection = true;
	}
	
	// We don't check for the pointer as we might want to set it to NULL
	m_pWave->SetCollection(pCollection);
}

// This can get called from the compression manager callbacks at the wave construction time
// At this time the node will have no wave but we don't want to see any asserts either.....
CCollection* CWaveNode::GetCollection() const
{
	if(m_pWave == NULL)
	{
		return NULL;
	}

	return m_pWave->GetCollection();
}

CWaveCtrl* CWaveNode::GetWaveEditor() const 
{
	return m_pWaveCtrl;
}

void CWaveNode::SetWaveEditor(CWaveCtrl* pWaveCtrl)
{
	// We don't check for NULL pointer since we might intentionally set it NULL
	m_pWaveCtrl = pWaveCtrl;
}

bool CWaveNode::ConfirmWaveDelete()
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return false;
	}

	CString					csReferPrompt, csDeletePrompt, csDeleteTitle, csConfirmPrompt;
	CString					csInstName;
	IDMUSProdNode*			pIRef;
	BSTR					bsInstName;
	int						iResponse;
	int						iInstrNameBufferLength;

	csConfirmPrompt.LoadString(IDS_WAVE_DELETE_CONFIRM);	//"Are you sure you want to delete this wave?"
	csDeleteTitle.LoadString(IDS_WAVE_DELETE_CONFIRM_TITLE);//Title for message box

	CCollection* pCollection = m_pWave->GetCollection();

	if(pCollection && pCollection->IsWaveReferenced(m_pWave, &pIRef) )
	{
		pIRef->GetNodeName(&bsInstName);
		
		// Calculate the buffer length required to convert from BSTR
		iInstrNameBufferLength = WideCharToMultiByte(CP_ACP,0,bsInstName,-1,NULL,0,NULL,NULL);
		
		// Do actual conversion
		WideCharToMultiByte(CP_ACP,0,bsInstName,-1,csInstName.GetBuffer(iInstrNameBufferLength),iInstrNameBufferLength,NULL,NULL);
		csInstName.ReleaseBuffer(-1);
		
		CString sWaveName = m_pWave->GetName();
		csReferPrompt.Format(IDS_WAVE_REFERENCE_PROMPT,sWaveName.GetBuffer(0),csInstName.GetBuffer(0));
		sWaveName.ReleaseBuffer();
		csDeletePrompt = csReferPrompt+csConfirmPrompt;
	}
	else
	{
		csDeletePrompt = csConfirmPrompt;
	}
	
	iResponse = AfxMessageBox(csDeletePrompt, MB_YESNO|MB_ICONWARNING);
	if ( iResponse == IDNO)
	{
		return false;
	}
	else
	{
		return true;
	}
}



HRESULT CWaveNode::OnViewProperties()
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

void CWaveNode::RefreshPropertyPage()
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	m_pWave->RefreshPropertyPage();
}


HRESULT CWaveNode::InitAndDownload()
{
	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		return E_FAIL;
	}

	// No ports to download to?
	if(m_pComponent->m_lstDLSPortDownload.IsEmpty())
	{
		return E_FAIL;
	}

	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_UNEXPECTED;
	}

	HRESULT	hr = S_OK;

	// Download the wave to all the ports
	POSITION position = m_pComponent->m_lstDLSPortDownload.GetHeadPosition();
	while(position)
	{
		IDirectMusicPortDownload* pIDMPortDownload = m_pComponent->m_lstDLSPortDownload.GetNext(position);
		ASSERT(pIDMPortDownload);

		hr = DM_Init(pIDMPortDownload);
		if(SUCCEEDED(hr))
		{
			hr = Download(pIDMPortDownload);
			if(!SUCCEEDED(hr))
			{
				if(hr & (DMUS_E_INSUFFICIENTBUFFER | DMUS_E_BUFFERNOTSET | DMUS_E_BUFFERNOTAVAILABLE))
				{
					// ashtat - BUGBUG! Need a better message...see Manbugs : 33331
					AfxMessageBox(IDS_ERR_DOWNLOAD_DLS_CONFIG);
				}
				AfxMessageBox(IDS_ERR_INIT_DOWNLOAD,MB_OK | MB_ICONEXCLAMATION);
			}
		}
		else if(hr & E_OUTOFMEMORY)
		{
			AfxMessageBox(IDS_ERR_WAVE_DOWNLOAD_FAILED, MB_OK | MB_ICONEXCLAMATION);
		}
	}

	return hr;
}

HRESULT CWaveNode::DownloadToAllPorts()
{
	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		return E_FAIL;
	}

	// No ports to download to?
	if(m_pComponent->m_lstDLSPortDownload.IsEmpty())
	{
		return E_FAIL;
	}

	HRESULT	hr = S_OK;

	// Download the wave to all the ports
	POSITION position = m_pComponent->m_lstDLSPortDownload.GetHeadPosition();
	while(position)
	{
		IDirectMusicPortDownload* pIDMPortDownload = m_pComponent->m_lstDLSPortDownload.GetNext(position);
		ASSERT(pIDMPortDownload);
		if(pIDMPortDownload)
		{
			Download(pIDMPortDownload);
		}
	}

	return S_OK;
}

HRESULT CWaveNode::UnloadFromAllPorts()
{
	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		return E_FAIL;
	}

	// No ports to download to?
	if(m_pComponent->m_lstDLSPortDownload.IsEmpty())
	{
		return E_FAIL;
	}

	HRESULT	hr = S_OK;

	// Download the wave to all the ports
	POSITION position = m_pComponent->m_lstDLSPortDownload.GetHeadPosition();
	while(position)
	{
		IDirectMusicPortDownload* pIDMPortDownload = m_pComponent->m_lstDLSPortDownload.GetNext(position);
		ASSERT(pIDMPortDownload);
		if(pIDMPortDownload)
		{
			Unload(pIDMPortDownload);
		}
	}

	return S_OK;
}


HRESULT	CWaveNode::DM_Init(IDirectMusicPortDownload* pIDMPortDownLoad)
{
	ASSERT(pIDMPortDownLoad);
	if(pIDMPortDownLoad == NULL)
		return E_POINTER;

	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return E_POINTER;

	if(SUCCEEDED(m_pWave->DM_Init(pIDMPortDownLoad)))
		return S_OK;

	return E_FAIL;
}

HRESULT CWaveNode::Download(IDirectMusicPortDownload* pIDMPortDownLoad)
{
	ASSERT(pIDMPortDownLoad);
	if(pIDMPortDownLoad == NULL)
		return E_POINTER;

	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return E_POINTER;

	// Okay to not download if the preference is set that way
	if(m_pComponent->IsAutoDownloadOkay() == FALSE)
	{
		return S_OK;
	}

	if(SUCCEEDED(m_pWave->Download(pIDMPortDownLoad)))
		return S_OK;

	return E_FAIL;
}

HRESULT	CWaveNode::Unload(IDirectMusicPortDownload* pIDMPortDownLoad)
{
	ASSERT(pIDMPortDownLoad);
	if(pIDMPortDownLoad == NULL)
		return E_POINTER;

	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return E_POINTER;

	if(SUCCEEDED(m_pWave->Unload(pIDMPortDownLoad)))
		return S_OK;

	return E_FAIL;
}


HRESULT CWaveNode::IsLoadingStereoWave(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain, bool& bStereo, bool& bTrueStereo, GUID& guidStereoWave, bool& bRightChannel)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pIRiffStream);
	if(pIRiffStream == NULL)
		return E_POINTER;

	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);

	// Remember where the stream pointer is positioned
	DWORD dwStreamPosition = StreamTell(pIStream);

	HRESULT hr = E_FAIL;
	DWORD cSize = 0;
	MMCKINFO ck;
	ck.ckid = mmioFOURCC('w','v','s','t');
	if(pIRiffStream->Descend(&ck, pckMain, MMIO_FINDCHUNK) == 0) 
	{
		DWORD cb = 0;
		bStereo = true;
		bTrueStereo = false;

		bRightChannel = false;
		
		hr = pIStream->Read(&bRightChannel, sizeof(bool), &cb);
		if(FAILED(hr) || cb != sizeof(bool))
		{
			pIStream->Release();
			pIRiffStream->Ascend(&ck, 0);
			return E_FAIL;
		}
		
		hr = pIStream->Read(&guidStereoWave, sizeof(GUID), &cb);
		if(FAILED(hr) || cb != sizeof(GUID))
		{
			pIStream->Release();
			pIRiffStream->Ascend(&ck, 0);
			return E_FAIL;
		}
	}
	else
	{
		ck.ckid = mmioFOURCC('f','m','t',' ');
		StreamSeek(pIStream, dwStreamPosition, STREAM_SEEK_SET);
		if(pIRiffStream->Descend(&ck, pckMain, MMIO_FINDCHUNK) == 0)
		{
			WAVEFORMATEX wfxTemp;
			if(ck.cksize < sizeof(WAVEFORMATEX))
			{
				cSize = ck.cksize;
			}
			else
			{
				cSize = sizeof(WAVEFORMATEX);
			}
			DWORD cb = 0;
			hr = pIStream->Read(&wfxTemp, cSize, &cb);
			if(FAILED(hr) || cb != cSize)
			{
				pIStream->Release();
				pIRiffStream->Ascend(&ck, 0);
				return E_FAIL;
			}

			if (wfxTemp.nChannels > 2)   // we only support mono and stereo files.
			{
				AfxMessageBox(IDS_WAVE_STEREO, MB_ICONEXCLAMATION); 
				pIRiffStream->Ascend(&ck, 0);
				pIStream->Release();
				return E_FAIL;
			}

			if(wfxTemp.nChannels == 2)
			{
				bStereo = true;
				bTrueStereo = true;
			}
			else
			{
				bStereo = false;
				bTrueStereo = false;
			}
		}
	}

	StreamSeek(pIStream, dwStreamPosition, STREAM_SEEK_SET );
	pIStream->Release();

	return hr;
}

bool CWaveNode::IsStereo()
{
	return m_bStereo;
}

bool CWaveNode::IsInACollection()
{
	return m_bInCollection;
}

HRESULT	CWaveNode::GetPlayTimeForTempo(int nTempo, DWORD& dwPlayTime)
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return E_FAIL;

	return m_pWave->GetPlayTimeForTempo(nTempo, dwPlayTime);
}

void CWaveNode::DrawWave(CDC* pDC, const CRect& rcClient)
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return;

	EnterCriticalSection(&m_CriticalSection);
	m_pWave->OnDraw(pDC, rcClient);
	LeaveCriticalSection(&m_CriticalSection);
}

HRESULT CWaveNode::GetObjectDescriptor( void* pObjectDesc )
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

	if( m_pWave == NULL )
	{
		return E_UNEXPECTED;
	}

	// Set values in DMUS_OBJECTDESC structure
//	pDMObjectDesc->dwValidData = (DMUS_OBJ_OBJECT | DMUS_OBJ_CLASS | DMUS_OBJ_VERSION | DMUS_OBJ_NAME );
	pDMObjectDesc->dwValidData = (DMUS_OBJ_OBJECT | DMUS_OBJ_CLASS | DMUS_OBJ_NAME );
	
	memcpy( &pDMObjectDesc->guidObject, &m_guidFile, sizeof(GUID) );
	memcpy( &pDMObjectDesc->guidClass, &CLSID_DirectSoundWave, sizeof(CLSID) );
//	pDMObjectDesc->vVersion.dwVersionMS = m_pWave->m_Version.dwVersionMS;
//	pDMObjectDesc->vVersion.dwVersionLS = m_pWave->m_Version.dwVersionLS;
	MultiByteToWideChar( CP_ACP, 0, m_pWave->GetName(), -1, pDMObjectDesc->wszName, DMUS_MAX_NAME );

	return S_OK;
}

void CWaveNode::SetFileName(CString sFileName)
{
	m_sFileName = sFileName;
}

CString CWaveNode::GetFileName()
{
	return m_sFileName;
}

GUID CWaveNode::GetGUID()
{
	return m_guidWaveNode;
}


void CWaveNode::CreateNewVersionGUID()
{
	::CoCreateGuid(&m_guidVersion);
}

GUID CWaveNode::GetVersionGUID()
{
	return m_guidVersion;
}


HRESULT CWaveNode::WriteDragChunkToStream(IDMUSProdRIFFStream* pIRiffStream)
{
	ASSERT(pIRiffStream);
	if(pIRiffStream == NULL)
	{
		return E_POINTER;
	}

	// We've already copied this node to the stream then....
	if(m_bDragCopy == true)
	{
		return S_OK;
	}

	// Get the GUID for the node and persist it in the data object
	GUID guidFile;
	if(FAILED(m_pComponent->m_pIFramework->GetNodeFileGUID(this, &guidFile)))
	{
		return E_FAIL;
	}

	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_FAIL;
	}

	// Create the drag-ck
	MMCKINFO ck;
	ck.ckid = FOURCC_DRAG;
	if(FAILED(pIRiffStream->CreateChunk(&ck, 0)))
	{
		pIStream->Release();
		return E_FAIL;
	}

	// Save the GUID
	DWORD cb = 0;
	HRESULT hr = pIStream->Write((LPCSTR)&guidFile, sizeof(GUID), &cb);
	if(FAILED(hr) || cb != sizeof(GUID))
	{
		pIStream->Release();
		return E_FAIL;
	}

	cb = 0;
	hr = pIStream->Write((LPCSTR)&m_guidWaveNode, sizeof(GUID), &cb);
	if(FAILED(hr) || cb != sizeof(GUID))
	{
		pIStream->Release();
		return E_FAIL;
	}

	if(FAILED(pIRiffStream->Ascend(&ck, 0)))
	{
		pIStream->Release();
		return E_FAIL;
	}
	
	m_bDragCopy = true;
	
	pIStream->Release();
	return S_OK;

}

bool CWaveNode::GetDragCopy()
{
	return m_bDragCopy;
}

void CWaveNode::SetDragCopy(bool bDragCopy)
{
	m_bDragCopy = bDragCopy;
}

void CWaveNode::SetDirtyFlag()
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	// Set the node's dirty flag only if it's NOT in a collection
	if(IsInACollection() == false)
	{
		m_bIsDirty = TRUE;
	}
	else
	{
		CCollection* pCollection = GetCollection();
		ASSERT(pCollection);
		if(pCollection)
		{
			pCollection->SetDirtyFlag();
		}
	}
}

void CWaveNode::ClearDirtyFlag()
{
	m_bIsDirty = FALSE;
}

IDirectSoundWave* CWaveNode::GetDirectSoundWave()
{
	return m_pIDirectSoundWave;
}

GUID CWaveNode::GetFileGUID()
{
	return m_guidFile;
}

void CWaveNode::SetFileGUID(GUID guidNew)
{
	if(m_pComponent->IsInWaveNodesList(guidNew))
	{
		::CoCreateGuid(&m_guidFile);
	}
	else
	{
		m_guidFile = guidNew;
	}
}

void CWaveNode::SetPreviousRegionLinkIDs(CWaveNode* pSourceWaveNode)
{
	ASSERT(pSourceWaveNode);
	if(pSourceWaveNode == NULL)
	{
		return;
	}

	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	CWave* pSourceWave = pSourceWaveNode->GetWave();
	ASSERT(pSourceWave);
	if(pSourceWave == NULL)
	{
		return;
	}

	if(IsStereo() && pSourceWaveNode->IsStereo())
	{
		CStereoWave* pSource = dynamic_cast<CStereoWave*>(pSourceWave);
		ASSERT(pSource);
		if(pSource == NULL)
		{
			return;
		}

		CStereoWave* pThisWave = dynamic_cast<CStereoWave*>(m_pWave);
		ASSERT(pThisWave);
		if(pThisWave == NULL)
		{
			return;
		}

		CMonoWave* pSrcLeftWave = pSource->GetLeftChannel();
		ASSERT(pSrcLeftWave);
		if(pSrcLeftWave == NULL)
		{
			return;
		}

		CMonoWave* pSrcRightWave = pSource->GetRightChannel();
		ASSERT(pSrcRightWave);
		if(pSrcRightWave == NULL)
		{
			return;
		}

		DWORD dwLeftID = pSrcLeftWave->GetWaveID();
		DWORD dwRightID = pSrcRightWave->GetWaveID();

		pThisWave->SetPreviousRegionLinkIDs(dwLeftID, dwRightID);
	}
	else if(!IsStereo() && !pSourceWaveNode->IsStereo())
	{
		CMonoWave* pSource = dynamic_cast<CMonoWave*>(pSourceWave);
		ASSERT(pSource);
		if(pSource == NULL)
		{
			return;
		}

		CMonoWave* pThisWave = dynamic_cast<CMonoWave*>(m_pWave);
		ASSERT(pThisWave);
		if(pThisWave == NULL)
		{
			return;
		}
		
		pThisWave->SetPreviousRegionLinkID(pSource->GetWaveID());
	}
	else
	{
		// This can't be!!!
		ASSERT(0);
	}
}

void CWaveNode::SetTransportName()
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		return;
	}

	IDMUSProdConductor* pIConductor = m_pComponent->m_pIConductor;
	ASSERT(pIConductor);
	if(pIConductor== NULL)
	{
		return;
	}

	// The actual transport name is collection:wavename
	CString sWaveName = m_pWave->GetName();

	CString sCollectionName = "";
	CCollection* pCollection = m_pWave->GetCollection();
	if(pCollection)
	{
		BSTR bstrCollectionName;
		if(SUCCEEDED(pCollection->GetNodeName(&bstrCollectionName)))
		{
			sCollectionName = bstrCollectionName;
			SysFreeString(bstrCollectionName);
		}
	}
	else
	{
		sCollectionName.LoadString(IDS_WAVE_OBJECT_TEXT);
	}

	CString sTransportName = sCollectionName + ":" + sWaveName;

	// Set the name for the transport
	BSTR bstrWaveName = sTransportName.AllocSysString();
	pIConductor->SetTransportName(this, bstrWaveName);
}

void CWaveNode::RegisterWithTransport()
{
	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		return;
	}

	// Register with the transport
	IDMUSProdConductor* pIConductor = m_pComponent->m_pIConductor;
	ASSERT(pIConductor);
	if(pIConductor)
	{
		pIConductor->RegisterTransport(this, 0);
		SetTransportName();
	}

}

void CWaveNode::UnRegisterFromTransport()
{
	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		return;
	}

	// Register with the transport
	IDMUSProdConductor* pIConductor = m_pComponent->m_pIConductor;
	ASSERT(pIConductor);
	if(pIConductor)
	{
		pIConductor->UnRegisterTransport(this);
	}
}

