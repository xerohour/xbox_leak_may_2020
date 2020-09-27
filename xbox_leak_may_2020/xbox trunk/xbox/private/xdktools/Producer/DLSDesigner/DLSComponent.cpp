// DLSComponent.cpp : implementation file
//

#include "stdafx.h"
#include "shlwapi.h"

#include "DlsDefsPlus.h"
#include "DLSDesignerDLL.h"
#include "Collection.h"
#include "Instrument.h"
#include "DLSDocType.h"
#include "WaveDocType.h"
#include "DLSComponent.h"
#include "AList.h"
#include "JazzDataObject.h"
#include "CollectionRef.h"
#include "WaveNode.h"
#include "Wave.h"
#include "WaveRefNode.h"
#include "MonoWave.h"
#include "StereoWave.h"
#include "Conductor.h"
#include "ConditionConfigToolbar.h"

#include <Process.h>
#include <dmksctrl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* HACKHACK
The DLS Designer now creates its own DMUS_APATH_DYNAMIC_STEREO audio path to play events on.
It eventually should copy the performance's default audio path.
*/

CWnd* CreateParentWindow( void )
{
	CWnd* pWndParent = new CWnd;
	if( pWndParent )
	{
		if( !pWndParent->CreateEx(0, AfxRegisterWndClass(0), NULL, WS_OVERLAPPED, 0, 0, 0, 0, NULL, NULL) )
		{
			delete pWndParent;
			pWndParent = NULL;
		}
	}
	return pWndParent;
}
/////////////////////////////////////////////////////////////////////////////
// CQueryValuePair Implmentation
CQueryValuePair::CQueryValuePair(CDLSQuery* pDLSQuery, DWORD dwValue) : 
m_pDLSQuery(pDLSQuery), m_dwValue(dwValue)
{
	ASSERT(pDLSQuery);
}

CQueryValuePair::~CQueryValuePair()
{
	m_pDLSQuery = NULL;
	m_dwValue = 0;
}

CDLSQuery* CQueryValuePair::GetQuery()
{
	ASSERT(m_pDLSQuery);
	return m_pDLSQuery;
}

DWORD CQueryValuePair::GetValue()
{
	ASSERT(m_pDLSQuery);
	return m_dwValue;
}

void CQueryValuePair::SetValue(DWORD dwValue)
{
	m_dwValue = dwValue;
}

/////////////////////////////////////////////////////////////////////////////
// CSystemConfiguration Implementation

CSystemConfiguration::CSystemConfiguration(bool bDefaultConfig) : m_bDefaultConfig(bDefaultConfig)
{
}

CSystemConfiguration::~CSystemConfiguration()
{
	// Delete the list
	while(!m_lstQueryValues.IsEmpty())
	{
		delete m_lstQueryValues.RemoveHead();
	}
}

CString CSystemConfiguration::GetName()
{
	return m_sName;
}

void CSystemConfiguration::SetName(CString sName)
{
	m_sName = sName;
}

bool CSystemConfiguration::IsDefault()
{
	return m_bDefaultConfig;
}

HRESULT	CSystemConfiguration::GetValueForQuery(CDLSQuery* pDLSQuery, DWORD& dwValue)
{
	ASSERT(pDLSQuery);
	if(pDLSQuery == NULL)
	{
		return E_POINTER;
	}

	return GetValueForQuery(pDLSQuery->GetGUID(), dwValue);
}



HRESULT	CSystemConfiguration::GetValueForQuery(GUID guidQuery, DWORD& dwValue)
{
	dwValue = 0;

	POSITION position = m_lstQueryValues.GetHeadPosition();
	while(position)
	{
		CQueryValuePair* pQueryValuePair = (CQueryValuePair*) m_lstQueryValues.GetNext(position);
		ASSERT(pQueryValuePair);
		if(pQueryValuePair)
		{
			CDLSQuery* pQuery = pQueryValuePair->GetQuery();
			ASSERT(pQuery);
			if(pQuery && ::IsEqualIID(pQuery->GetGUID(), guidQuery))
			{
				dwValue = pQueryValuePair->GetValue();
				return S_OK;
			}
		}
	}

	return E_FAIL;
}


HRESULT CSystemConfiguration::SetValueForQuery(CDLSQuery* pDLSQuery, DWORD dwValue)
{
	ASSERT(pDLSQuery);
	if(pDLSQuery == NULL)
	{
		return E_POINTER;
	}

	CQueryValuePair* pQueryValuePair = GetQueryValuePair(pDLSQuery);
	if(pQueryValuePair)
	{
		pQueryValuePair->SetValue(dwValue);
		return S_OK;
	}

	return E_FAIL;
}


CQueryValuePair* CSystemConfiguration::GetQueryValuePair(CDLSQuery* pDLSQuery)
{
	ASSERT(pDLSQuery);
	if(pDLSQuery == NULL)
	{
		return NULL;
	}

	POSITION position = m_lstQueryValues.GetHeadPosition();
	while(position)
	{
		CQueryValuePair* pQueryValuePair = (CQueryValuePair*) m_lstQueryValues.GetNext(position);
		ASSERT(pQueryValuePair);
		CDLSQuery* pQueryInPair = pQueryValuePair->GetQuery();
		if(pQueryInPair == pDLSQuery)
		{
			return pQueryValuePair;
		}
	}

	return NULL;
}


HRESULT CSystemConfiguration::AddQueryValue(CQueryValuePair* pQueryValuePair)
{
	ASSERT(pQueryValuePair);
	if(pQueryValuePair == NULL)
	{
		return E_POINTER;
	}

	m_lstQueryValues.AddTail(pQueryValuePair);
	return S_OK;
}


HRESULT CSystemConfiguration::RemoveQueryValuePair(CQueryValuePair* pQueryValuePair)
{
	ASSERT(pQueryValuePair);
	if(pQueryValuePair == NULL)
	{
		return E_POINTER;
	}

	POSITION position = m_lstQueryValues.Find(pQueryValuePair);
	if(position)
	{
		m_lstQueryValues.RemoveAt(position);
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CSystemConfiguration::RemoveQueryValuePair(GUID guidQuery)
{
	POSITION position = m_lstQueryValues.GetHeadPosition();
	while(position)
	{
		// Keep the position; we might need it to remove it from the list
		POSITION oldPosition = position;

		CQueryValuePair* pQueryValuePair = (CQueryValuePair*) m_lstQueryValues.GetNext(position);
		ASSERT(pQueryValuePair);
		if(pQueryValuePair)
		{
			CDLSQuery* pDLSQuery = pQueryValuePair->GetQuery();
			ASSERT(pDLSQuery);
			if(pDLSQuery && ::IsEqualIID(pDLSQuery->GetGUID(), guidQuery))
			{
				m_lstQueryValues.RemoveAt(oldPosition);
				delete pQueryValuePair;
				return S_OK;
			}
		}
	}

	return E_FAIL;
}

CPtrList& CSystemConfiguration::GetQueryValueList()
{
	return m_lstQueryValues;
}


/////////////////////////////////////////////////////////////////////////////
// CDLSComponent constructor/destructor

CDLSComponent::CDLSComponent() :
m_pCopyDataObject(NULL),
m_bInReferencedLoad(FALSE),
m_bShowGetFormatErrorMessage(true),
m_bNoShowCollectionPatchConflict(false),
m_bNoShowUserPatchConflict(false),
#ifdef DMP_XBOX
m_bNoShowBadXboxLoopWarning(false),
#endif // DMP_XBOX
m_bLongLoopOK(FALSE),
m_bInCleanup(FALSE),
m_bDownloadCollection(TRUE)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    m_dwRef = 0;
	
	m_pIFramework = NULL;
	m_pIConductor = NULL;

	m_pIDirectMusic = NULL;

	m_pIPerformance = NULL;
	m_pISynthPort = NULL;
	m_pIAudioPath = NULL;

	m_pIDLSDocType8 = NULL;
	m_pIWaveDocType8 = NULL;

	m_pConditionConfigToolbar = NULL;

	m_nFirstCollectionImage = 0;
	m_nFirstInstrumentImage = 0;
	m_nFirstArticulationImage = 0;
	m_nFirstArticulationGrayedImage = 0;
	m_nFirstRegionImage = 0;
	m_nFirstRegionGrayedImage = 0;
	m_nFirstWaveImage = 0;
	m_nFirstStereoWaveImage = 0;
	m_nFirstWaveRefImage = 0;
	m_nFirstStereoWaveRefImage = 0;
	m_nFirstFolderImage = 0;
	m_nFirstFolderGrayedImage = 0;

	m_nNextDLS = 0;
	m_nNextWave = 0;

	m_dwInstrumentIndex = 0;

    m_cfCollection = 0;
    m_cfWave = 0;
	m_cfInstrument = 0;

	m_bDummyMSB = 0x7F;
	m_bDummyLSB = 0x7F;
	m_bDummyPatch = 0x7F;

	// Initialize m_bSnapToZero with the value in the registry
	InitSnapToZero();
}

CDLSComponent::~CDLSComponent()
{
	ReleaseAll();
}

void CDLSComponent::ReleaseAll( void )
{
	TCHAR szValueName[MAX_PATH];
	_tcscpy(szValueName, _T("SnapToZero"));
	WriteToRegistry(szValueName, REG_DWORD, (BYTE*)(&m_bSnapToZero), sizeof(BOOL));

	CleanUp();

	if( m_pIFramework )
	{
		if(m_pConditionConfigToolbar)
		{
			if( ::IsWindow(m_pConditionConfigToolbar->m_hWnd) )
			{
				m_pConditionConfigToolbar->DestroyWindow();
			}

			m_pIFramework->RemoveToolBar(m_pConditionConfigToolbar);
		}

		m_pIFramework->Release();
		m_pIFramework = NULL;
	}

	if( m_pIDLSDocType8 )
	{
		m_pIDLSDocType8->Release();
		m_pIDLSDocType8 = NULL;
	}

	if( m_pIWaveDocType8 )
	{
		m_pIWaveDocType8->Release();
		m_pIWaveDocType8 = NULL;
	}

	while( !m_lstDLSPortDownload.IsEmpty() )
	{
		m_lstDLSPortDownload.RemoveHead()->Release();
	}

	if( m_pIAudioPath )
	{
		m_pIAudioPath->Release();
		m_pIAudioPath = NULL;
	}
	if( m_pISynthPort )
	{
		m_pISynthPort->Release();
		m_pISynthPort= NULL;
	}
	if (m_pIPerformance) 
	{
		m_pIPerformance->Release();
		m_pIPerformance = NULL;
	}
	if( m_pIDirectMusic )
	{
		m_pIDirectMusic->Release();
		m_pIDirectMusic = NULL;
	}
	if( m_pIConductor )
	{
		m_pIConductor->Release();
		m_pIConductor = NULL;
	}

	// Update the configs in the registry before we get destroyed
	UpdateConfigsInRegistry();

	DeleteGUIDList();
	DeleteQueryList();
	DeleteConfigList();
}

void CDLSComponent::WriteToRegistry(TCHAR* pszValueName, UINT nValueType, BYTE* pbValue, DWORD cbData, TCHAR* pszSubPath)
{
	TCHAR szComponentPath[MAX_PATH];
	_tcscpy( szComponentPath, _T("Software\\Microsoft\\DMUSProducer\\DLSDesigner\\"));

	if(pszSubPath != NULL)
	{
		_tcscat(szComponentPath, pszSubPath);
	}
	
	HKEY	hkeyOpen;
	LPDWORD	lpdwDisposition = NULL;
	if(::RegCreateKeyEx(HKEY_CURRENT_USER, 
						szComponentPath, 
						0, 
						0,
						REG_OPTION_NON_VOLATILE, 
						KEY_ALL_ACCESS, 
						NULL, &hkeyOpen, 
						lpdwDisposition) == ERROR_SUCCESS)
	{
		if(RegSetValueEx(hkeyOpen, pszValueName, 0, nValueType, pbValue, cbData) != ERROR_SUCCESS)
		{
			TRACE("CDLSComponent::WriteToRegistry() - Registry Set Value FAILED!!");
		}

		::RegCloseKey(hkeyOpen);
	}
}


void CDLSComponent::WriteQueryToRegistry(const GUID& guidQuery, UINT nNameID)
{
	CString sKeyName;
	sKeyName.LoadString(nNameID);
	WriteQueryToRegistry(guidQuery, sKeyName);
}


void CDLSComponent::WriteQueryToRegistry(const GUID& guidQuery, CString sKeyName)
{
	LPOLESTR psz;
	if( SUCCEEDED( ::StringFromIID(guidQuery, &psz) ) )
	{
		TCHAR szGuid[100];
		WideCharToMultiByte( CP_ACP, 0, psz, -1, szGuid, sizeof(szGuid), NULL, NULL );
		CoTaskMemFree( psz );

		WriteToRegistry(szGuid, REG_SZ, (BYTE*)((LPCSTR)sKeyName), (_tcslen(sKeyName) + 1), "Queries\\");
	}

}



void CCollectionList::AddTail(CCollection *pINode) 
{ 
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	AList::AddTail((AListItem *) pINode);
}

CCollection *CCollectionList::GetItem(LONG lIndex) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	return (CCollection *)AList::GetItem(lIndex);
}

CCollection *CCollectionList::GetHead() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	return (CCollection *)AList::GetHead();
}

CCollection *CCollectionList::RemoveHead() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	return (CCollection *) AList::RemoveHead();
}

void CCollectionList::Remove(CCollection *pINode) 
{ 
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	AList::Remove((AListItem *) pINode);
}

/////////////////////////////////////////////////////////////////////////////
// CDLSComponent IUnknown implementation

HRESULT CDLSComponent::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    if(::IsEqualIID(riid, IID_IDMUSProdComponent) ||
	   ::IsEqualIID(riid, IID_IUnknown))
    {
        AddRef();
        *ppvObj = (IDMUSProdComponent*) this;
        return S_OK;
    }

	if(::IsEqualIID(riid, IID_IDMUSProdPortNotify))
    {
        AddRef();
        *ppvObj = (IDMUSProdPortNotify*) this;
        return S_OK;
    }

	if(::IsEqualIID(riid, IID_IDMUSProdDLSNotify))
    {
        AddRef();
        *ppvObj = (IDMUSProdDLSNotify*) this;
        return S_OK;
    }

	if(::IsEqualIID(riid, IID_IDMUSProdNotifySink))
    {
        AddRef();
        *ppvObj = (IDMUSProdNotifySink*) this;
        return S_OK;
    }

	if(::IsEqualIID(riid, IID_IDLSReferenceLoadNotify))
    {
        AddRef();
        *ppvObj = (IDLSReferenceLoadNotify*) this;
        return S_OK;
    }

	if(::IsEqualIID(riid, IID_IDMUSProdRIFFExt))
	{
		AddRef();
		*ppvObj = (IDMUSProdRIFFExt*) this;
		return S_OK;
	}

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CDLSComponent::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CDLSComponent::Release()
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
// CDLSComponent IDMUSProdComponent implementation

/////////////////////////////////////////////////////////////////////////////
// CDLSComponent IDMUSProdComponent::Initialize

HRESULT CDLSComponent::Initialize( IDMUSProdFramework* pIFramework, BSTR* pbstrErrMsg )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Validate arguments
	ASSERT(pIFramework);
	ASSERT(pbstrErrMsg);
	
	IDMUSProdComponent* pIComponent = NULL;
	TCHAR achErrMsg[MID_BUFFER];
	CString strErrMsg;

	if( m_pIFramework )		// already initialized
	{
		return S_OK;
	}

	theApp.SetFramework(pIFramework);
	m_pIFramework = pIFramework;
	m_pIFramework->AddRef();


	// Get IConductor and IAAEngine interface pointers 
	if( !SUCCEEDED ( pIFramework->FindComponent( CLSID_CConductor,  &pIComponent ) )
	||  !SUCCEEDED ( pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&m_pIConductor ) ) )
	{
		ReleaseAll();
		if( pIComponent )
		{
			pIComponent->Release();
		}
		::LoadString( theApp.m_hInstance, IDS_ERR_MISSING_CONDUCTOR, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	OpenPort();

	if(m_pISynthPort)
	{
		if(FAILED(UpdateDLSPortList()))
		{
			m_pISynthPort->Release();
			m_pISynthPort = NULL;
		}

	}

	// Get the download status for the DLS downloads
	IDMUSProdConductor8* pIConductor8 = NULL;
	if(FAILED(m_pIConductor->QueryInterface(IID_IDMUSProdConductor8, (void**)&pIConductor8)))
	{
		m_pISynthPort->Release();
		m_pISynthPort = NULL;
		return E_FAIL;
	}

	if(FAILED(pIConductor8->GetDownloadCustomDLSStatus(&m_bAutoDownloadDLS)))
	{
		m_bAutoDownloadDLS = TRUE;
	}

	pIConductor8->Release();


	pIComponent->Release();

	// Register applicable doc types with the Framework 
	m_pIDLSDocType8 = new CDLSDocType( this );
    if( m_pIDLSDocType8 == NULL )
    {
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_MEMORY, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
        return E_OUTOFMEMORY;
    }

	m_pIDLSDocType8->AddRef();
	
	if( !SUCCEEDED ( pIFramework->AddDocType(m_pIDLSDocType8) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_DOCTYPE, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	m_pIWaveDocType8 = new CWaveDocType(this);
	if(m_pIWaveDocType8 == NULL)
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_MEMORY, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
        return E_OUTOFMEMORY;
    }

	m_pIWaveDocType8->AddRef();
	
	if( !SUCCEEDED ( pIFramework->AddDocType(m_pIWaveDocType8) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_DOCTYPE, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Add applicable images to the Project Tree control's image list 
	if( !SUCCEEDED ( AddNodeImageLists() ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_IMAGELIST, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

    // Register the clipboard formats.
	m_cfProducerFile = ::RegisterClipboardFormat( CF_DMUSPROD_FILE );
    m_cfCollection = ::RegisterClipboardFormat( CF_DLS_COLLECTION );
	m_cfWave = ::RegisterClipboardFormat( CF_DLS_WAVE );
	m_cfInstrument = ::RegisterClipboardFormat( CF_DLS_INSTRUMENT );
    if (m_cfCollection == 0 || m_cfWave == 0 || m_cfProducerFile == 0 || m_cfInstrument == 0)
    {
		ReleaseAll();
        //return FALSE;
		return E_FAIL;
    }

	// Tell Framework that the CF_DLS_COLLECTION format can be used to create a .dls
	CString strExt = _T(".dlp");
	BSTR bstrExt = strExt.AllocSysString();
	if( !SUCCEEDED ( pIFramework->RegisterClipFormatForFile(m_cfCollection, bstrExt) ) )
	{
		ReleaseAll();
		return E_FAIL;
	}

	// Tell Framework that the CF_DLS_WAVE format can be used to create a .wav
	strExt = _T(".wvp");
	bstrExt = strExt.AllocSysString();
	if( !SUCCEEDED ( pIFramework->RegisterClipFormatForFile(m_cfWave, bstrExt) ) )
	{
		ReleaseAll();
		return E_FAIL;
	}

#ifndef DMP_XBOX
	// Register the default DLS Queries (Supported by DMusic) if they aren't already in the registry
	WriteQueryToRegistry(DLSID_GMInHardware, IDS_QUERY_GMINHARDWARE);
	WriteQueryToRegistry(DLSID_GSInHardware, IDS_QUERY_GSINHARDWARE);
	WriteQueryToRegistry(DLSID_XGInHardware, IDS_QUERY_XGINHARDWARE);
	WriteQueryToRegistry(DLSID_SupportsDLS1, IDS_QUERY_SUPPORTSDLS1);
	WriteQueryToRegistry(DLSID_SupportsDLS2, IDS_QUERY_SUPPORTSDLS2);
	WriteQueryToRegistry(DLSID_SampleMemorySize, IDS_QUERY_SAMPLEMEMORYSIZE);
	WriteQueryToRegistry(DLSID_ManufacturersID, IDS_QUERY_MANUFACTURERSID);
	WriteQueryToRegistry(DLSID_ProductID, IDS_QUERY_PRODUCTID);
	WriteQueryToRegistry(DLSID_SamplePlaybackRate, IDS_QUERY_SAMPLEPLAYBACKRATE);

	InitQueryList();

	m_pConditionConfigToolbar = new CConditionConfigToolbar(this);
	if(m_pConditionConfigToolbar)
	{
		CWnd* pParentWnd = CreateParentWindow();
		if(pParentWnd == NULL)
		{
			delete m_pConditionConfigToolbar;
			m_pConditionConfigToolbar = NULL;
		}
		else
		{
			CRect rect(0, 0, 0, 0);
			if(m_pConditionConfigToolbar->Create(WS_CHILD, rect, pParentWnd, IDC_CONDITION_CONFIG_TOOLBAR) == 0)
			{
				pParentWnd->DestroyWindow();
				delete m_pConditionConfigToolbar;
				m_pConditionConfigToolbar = NULL;
			}
			else
			{
				HRESULT hr = pIFramework->AddToolBar(m_pConditionConfigToolbar);
				//  IDMUSProdFramework::AddToolBar reassigns parent so it is ok to destroy pWndParent
				if(pParentWnd->GetSafeHwnd())
				{
					pParentWnd->DestroyWindow();
				}
				delete pParentWnd;
				pParentWnd = NULL;

				if(FAILED(hr))
				{
					// Clean up
					if(m_pConditionConfigToolbar->m_hWnd)
					{
						m_pConditionConfigToolbar->DestroyWindow();
					}
					delete m_pConditionConfigToolbar;
					m_pConditionConfigToolbar = NULL;
				}
				else
				{
					// Load up saved configs from the registry
					LoadConfigurations();
					
					// Create the default configurations for active ports
					CreateDefaultConfigs();

					// Succeeded
					RefreshConfigToolbar();

					// Set the first config as current
					CSystemConfiguration* pConfig = (CSystemConfiguration*) m_lstConfigurations.GetHead();
					ASSERT(pConfig);
					m_pConditionConfigToolbar->SetCurrentConfig(pConfig);
				}
			}

		}
	}
#endif //DMP_XBOX
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDLSComponent IDMUSProdComponent::CleanUp

HRESULT CDLSComponent::CleanUp( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_bInCleanup = TRUE;

	if(m_pCopyDataObject)
	{
		if(S_OK == OleIsCurrentClipboard(m_pCopyDataObject))
		{
			OleFlushClipboard();
		}
		m_pCopyDataObject->Release();
		m_pCopyDataObject = NULL;
	}
	
	CCollection *pCollection;

	while( !m_CollectionList.IsEmpty() )
	{
		pCollection = m_CollectionList.RemoveHead();
		pCollection->Release();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDLSComponent IDMUSProdComponent::GetName

HRESULT CDLSComponent::GetName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CString strName;
	TCHAR achBuffer[BUFFER_128];

	if(::LoadString(theApp.m_hInstance, IDS_DLS_COMPONENT_NAME, achBuffer, BUFFER_128))
	{
		strName = achBuffer;
	}

    *pbstrName = strName.AllocSysString();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CDLSComponent IDMUSProdComponent::CreateReferenceNode

HRESULT CDLSComponent::AllocReferenceNode( GUID guidRefNodeId, IDMUSProdNode** ppIRefNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if( ppIRefNode == NULL )
	{
		return E_POINTER;
	}

	*ppIRefNode = NULL;

	// Make sure Component can create Nodes of type guidRefNodeId
	if( IsEqualGUID ( guidRefNodeId, GUID_CollectionRefNode ) ) 
	{
		// Create RefNode
		CCollectionRef* pCollectionRef = new CCollectionRef(this);
		if( pCollectionRef == NULL )
		{
			return E_OUTOFMEMORY ;
		}

		*ppIRefNode = (IDMUSProdNode *)pCollectionRef;

		return S_OK;
	}
	else if( IsEqualGUID ( guidRefNodeId, GUID_WaveRefNode ) ) 
	{
		// Create RefNode
		CWaveRefNode* pWaveRefNode = new CWaveRefNode(this);
		if( pWaveRefNode == NULL )
		{
			return E_OUTOFMEMORY ;
		}

		*ppIRefNode = (IDMUSProdNode *)pWaveRefNode;

		return S_OK;
	}

	return E_INVALIDARG;
}

/////////////////////////////////////////////////////////////////////////////
// CDLSComponent IDMUSProdComponent::OnActivateApp

HRESULT CDLSComponent::OnActivateApp(BOOL fActivate)
{
	return E_NOTIMPL;
}



/////////////////////////////////////////////////////////////////////////////
// CDLSComponent IDMUSProdPortNotify Implementation

/////////////////////////////////////////////////////////////////////////////
// CDLSComponent::OnOutputPortsRemoved
HRESULT CDLSComponent::OnOutputPortsRemoved(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// ...first unloaad all the downloaded collections from the port...
	CCollection* pCollection = m_CollectionList.GetHead();
	while (pCollection)
	{
		pCollection->Unload();
		pCollection = pCollection->GetNext();
	}

	// Now unload all the tand alone waves
	POSITION position = m_lstWaveNodes.GetHeadPosition();
	while(position)
	{
		CWaveNode* pWaveNode = (CWaveNode*)m_lstWaveNodes.GetNext(position);
		ASSERT(pWaveNode);
		if(pWaveNode && pWaveNode->IsInACollection() == false)
		{
			pWaveNode->UnloadFromAllPorts();
		}
	}


	while( !m_lstDLSPortDownload.IsEmpty() )
	{
		m_lstDLSPortDownload.RemoveHead()->Release();
	}
	
	if(m_pISynthPort)
	{
		m_pISynthPort->Release();
		m_pISynthPort = NULL;
	}

	if(m_pIAudioPath)
	{
		m_pIAudioPath->Release();
		m_pIAudioPath = NULL;
	}


	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CDLSComponent::OnOutputPortsChanged
HRESULT CDLSComponent::OnOutputPortsChanged(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( !m_pIConductor ) 
	{
		return S_FALSE;
	}

	// Release the current output ports
	OnOutputPortsRemoved();

	// Create our own private audio path (if necessary)
	if( m_pIAudioPath == NULL )
	{
		// TODO: Replace with code that duplicates the default performance audio path (Needs API work)
		if( FAILED( m_pIPerformance->CreateStandardAudioPath( DMUS_APATH_DYNAMIC_STEREO, 10, TRUE, &m_pIAudioPath ) ) )
		{
			// Only warn the user if we have any collections open
			if( !m_CollectionList.IsEmpty() )
			{
				AfxMessageBox(IDS_ERR_NO_PORT, MB_OK | MB_ICONEXCLAMATION);
			}

			return S_OK;
		}
	}

	// Now, see what PChannel 0 really maps to
	DWORD dwOutputPChannel;
	HRESULT hr = m_pIAudioPath->ConvertPChannel( 0, &dwOutputPChannel );
	if( FAILED( hr ) )
	{
		// Only warn the user if we have any collections open
		if( !m_CollectionList.IsEmpty() )
		{
			AfxMessageBox(IDS_ERR_NO_PORT, MB_OK | MB_ICONEXCLAMATION);
		}

		return S_OK;
	}

	// Now, see what synth the audio path is using
	if( FAILED( m_pIPerformance->PChannelInfo( dwOutputPChannel, &m_pISynthPort, NULL, NULL ) ) )
	{
		// Only warn the user if we have any collections open
		if( !m_CollectionList.IsEmpty() )
		{
			AfxMessageBox(IDS_ERR_NO_PORT, MB_OK | MB_ICONEXCLAMATION);
		}

		return S_OK;
	}

	if(FAILED(UpdateDLSPortList()))
	{
		m_pISynthPort->Release();
		m_pISynthPort = NULL;
		return E_FAIL;
	}

	// If the downloadport list is not empty then...
	if(!m_lstDLSPortDownload.IsEmpty())
	{
		// ...download all the opened collections to the port...
		CCollection* pCollection = m_CollectionList.GetHead();
		while (pCollection)
		{
			if(FAILED(pCollection->Download()))
			{
				return E_FAIL;
			}

			pCollection = pCollection->GetNext();
		}

		// Now download all the standalone waves
		POSITION position = m_lstWaveNodes.GetHeadPosition();
		while(position)
		{
			CWaveNode* pWaveNode = (CWaveNode*)m_lstWaveNodes.GetNext(position);
			ASSERT(pWaveNode);
			if(pWaveNode && pWaveNode->IsInACollection() == false)
			{
				if(FAILED(pWaveNode->InitAndDownload()))
				{
					return E_FAIL;
				}
			}
		}

	}

	CreateDefaultConfigs();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDLSComponent IDMUSProdDLSNotify Implementation

/////////////////////////////////////////////////////////////////////////////
// CDLSComponent::OnDownloadCustomDLS 

HRESULT CDLSComponent::OnDownloadCustomDLS(BOOL fDownloadCustomDLS)
{
	m_bAutoDownloadDLS = fDownloadCustomDLS;

	return UpdateDLSDownloads();
}

/////////////////////////////////////////////////////////////////////////////
// CDLSComponent::OnDownloadGM

HRESULT CDLSComponent::OnDownloadGM(BOOL fDownloadGM)
{
	// Conductor takes care of downloading the GM set
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CDLSComponent::OnUpdate

HRESULT CDLSComponent::OnUpdate(IDMUSProdNode *pIDocRootNode, GUID guidUpdateType, void *pData)
{
	// GUID_DownloadOnLoadRIFFChunk
	if (IsEqualGUID(GUID_DownloadOnLoadRIFFChunk, guidUpdateType))
	{
		// pData should not be NULL if we are passed GUID_DownloadOnLoadRIFFChunk.
		ASSERT(NULL != pData);
		if(NULL == pData)
		{
			return E_INVALIDARG;
		}

		m_bDownloadCollection = *(BOOL*)pData;
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CDLSComponent IDLSReferenceLoadNotify implementation

/////////////////////////////////////////////////////////////////////////////
// CDLSComponent::MarkLoadStart()

HRESULT CDLSComponent::MarkLoadStart()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	m_bInReferencedLoad = TRUE;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CDLSComponent::MarkLoadEnd()

HRESULT CDLSComponent::MarkLoadEnd()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	m_bInReferencedLoad = FALSE;
	DeleteGUIDList();
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDLSComponent IDMUSProdRIFFExt Implementation

////////////////////////////////////////////////////////////////////////////
// CDLSComponent::LoadRIFFChunk

HRESULT CDLSComponent::LoadRIFFChunk( IStream* pIStream, IDMUSProdNode** ppITopNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIStream != NULL );
	if( pIStream == NULL )
	{
		return E_POINTER;
	}

	if( ppITopNode == NULL )
	{
		return E_POINTER;
	}
	*ppITopNode = NULL;

	// Create a RIFF stream
	IDMUSProdRIFFStream* pIRiffStream;
    if( FAILED ( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return E_FAIL;
	}

	// Store the stream position
	DWORD dwPos = StreamTell( pIStream );

	HRESULT hr = E_FAIL;

    // DLS Collections
	MMCKINFO ckMain;
	ckMain.fccType = FOURCC_DLS;
    if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
    {
		StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );

		CCollection* pCollection = new CCollection( this );
		if( pCollection )
		{
			if( SUCCEEDED ( pCollection->Load( pIStream ) ) )
			{
				*ppITopNode = (IDMUSProdNode *)pCollection;
				hr = S_OK;
			}
			else
			{
				pCollection->Release();
			}
			// Reset download collection flag so that we download the 
			// next time if flag was not set to FALSE in the OnUpdate() call.
			m_bDownloadCollection = TRUE;
		}
    }
	else
	{
		CString sFileName;
		IDMUSProdPersistInfo* pIJazzPersistInfo;
		if(SUCCEEDED(pIStream->QueryInterface(IID_IDMUSProdPersistInfo,(void**) &pIJazzPersistInfo)))
		{
			BSTR bstrFileName;
			pIJazzPersistInfo->GetFileName(&bstrFileName);
			sFileName = bstrFileName;
			::SysFreeString(bstrFileName);
			pIJazzPersistInfo->Release();
		}

		// Waves
		StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );

		ckMain.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
		{
			StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );

			CWaveNode* pWaveNode = NULL;
			if(SUCCEEDED(CWaveNode::CreateWaveNode(this, pIStream, sFileName, &pWaveNode, NULL, false)))
			{
				// Download the wave to all the ports
				if(FAILED(pWaveNode->InitAndDownload()))
				{
					pWaveNode->Release();
					pWaveNode = NULL;
					return E_FAIL;
				}

				*ppITopNode = pWaveNode;
				
				// Register it with the transport
				pWaveNode->RegisterWithTransport();

				hr = S_OK;
			}
		}
	}

    pIRiffStream->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDLSComponent additional functions

/////////////////////////////////////////////////////////////////////////////
// CDLSComponent::AddNodeImageLists

HRESULT CDLSComponent::AddNodeImageLists( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CImageList lstImages;
	HICON hIcon;

	// Create image list for Collection nodes
	lstImages.Create( 16, 16, ILC_COLOR16, 2, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_COLLECTION) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_COLLECTION_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( !SUCCEEDED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstCollectionImage ) ) )
	{
		return E_FAIL;
	}

	// Create image list for Collection Reference nodes
	lstImages.Create( 16, 16, ILC_COLOR16, 2, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_COLLECTION_REF) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_COLLECTION_REF_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( !SUCCEEDED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstCollectionRefImage ) ) )
	{
		return E_FAIL;
	}
	
	// Create image list for Instrument nodes
	lstImages.Create( 16, 16, ILC_COLOR16, 2, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_INSTRUMENT) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_INSTRUMENT_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( !SUCCEEDED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstInstrumentImage ) ) )
	{
		return E_FAIL;
	}
	
	// Create image list for Articulation nodes
	lstImages.Create( 16, 16, ILC_COLOR16, 2, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_ARTICULATION) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_ARTICULATION_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( !SUCCEEDED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstArticulationImage ) ) )
	{
		return E_FAIL;
	}

	// Create image list for grayed out Articulation nodes
	lstImages.Create( 16, 16, ILC_COLOR16, 2, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_ARTICULATION_GRAY) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_ARTICULATION_GRAY_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( !SUCCEEDED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstArticulationGrayedImage ) ) )
	{
		return E_FAIL;
	}

	// Create image list for Region nodes
	lstImages.Create( 16, 16, ILC_COLOR16, 2, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_REGION) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_REGION_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( !SUCCEEDED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstRegionImage ) ) )
	{
		return E_FAIL;
	}

	// Create the grayed out region node images
	lstImages.Create( 16, 16, ILC_COLOR16, 2, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_REGION_GRAY) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_REGION_GRAY_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( !SUCCEEDED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstRegionGrayedImage ) ) )
	{
		return E_FAIL;
	}

	// Create image list for Wave nodes
	lstImages.Create( 16, 16, ILC_COLOR16, 2, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_WAVE) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_WAVE_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( !SUCCEEDED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstWaveImage ) ) )
	{
		return E_FAIL;
	}

	// Create image list for Wave nodes
	lstImages.Create( 16, 16, ILC_COLOR16, 2, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_WAVE_STEREO) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_WAVE_STEREO_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( !SUCCEEDED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstStereoWaveImage ) ) )
	{
		return E_FAIL;
	}


	// Create image list of generic folder images
	lstImages.Create( 16, 16, ILC_COLOR16, 2, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_FOLDER) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_FOLDER_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( !SUCCEEDED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstFolderImage ) ) )
	{
		return E_FAIL;
	}

	// Create image list of grayed out generic folder images
	lstImages.Create( 16, 16, ILC_COLOR16, 2, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_FOLDER_GRAY) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_FOLDER_GRAY_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( !SUCCEEDED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstFolderGrayedImage ) ) )
	{
		return E_FAIL;
	}



	// Create image list for WaveRef nodes
	lstImages.Create( 16, 16, ILC_COLOR16, 2, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_WAVE_REF) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_WAVE_REF_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( !SUCCEEDED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstWaveRefImage ) ) )
	{
		return E_FAIL;
	}

	// Create image list for WaveRef nodes
	lstImages.Create( 16, 16, ILC_COLOR16, 2, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_WAVE_STEREO_REF) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_WAVE_STEREO_REF_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( !SUCCEEDED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstStereoWaveRefImage ) ) )
	{
		return E_FAIL;
	}
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDLSComponent::GetCollectionImageIndex

HRESULT CDLSComponent::GetCollectionImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*pnFirstImage = m_nFirstCollectionImage;
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CDLSComponent::GetCollectionRefImageIndex

HRESULT CDLSComponent::GetCollectionRefImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*pnFirstImage = m_nFirstCollectionRefImage;
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CDLSComponent::GetArticulationImageIndex

HRESULT CDLSComponent::GetArticulationImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*pnFirstImage = m_nFirstArticulationImage;
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDLSComponent::GetArticulationGrayedImageIndex

HRESULT CDLSComponent::GetArticulationGrayedImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*pnFirstImage = m_nFirstArticulationGrayedImage;
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CDLSComponent::GetRegionImageIndex

HRESULT CDLSComponent::GetRegionImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*pnFirstImage = m_nFirstRegionImage;
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CDLSComponent::GetRegionGrayedImageIndex

HRESULT CDLSComponent::GetRegionGrayedImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*pnFirstImage = m_nFirstRegionGrayedImage;
	
	return S_OK;
}


////////////////////////////////////////////////////////////////////////////
// CDLSComponent::GetWaveImageIndex

HRESULT CDLSComponent::GetWaveImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*pnFirstImage = m_nFirstWaveImage;
	
	return S_OK;
}


////////////////////////////////////////////////////////////////////////////
// CDLSComponent::GetStereoWaveImageIndex

HRESULT CDLSComponent::GetStereoWaveImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*pnFirstImage = m_nFirstStereoWaveImage;
	
	return S_OK;
}


////////////////////////////////////////////////////////////////////////////
// CDLSComponent::GetWaveRefImageIndex

HRESULT CDLSComponent::GetWaveRefImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*pnFirstImage = m_nFirstWaveRefImage;
	
	return S_OK;
}


////////////////////////////////////////////////////////////////////////////
// CDLSComponent::GetStereoWaveRefImageIndex

HRESULT CDLSComponent::GetStereoWaveRefImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*pnFirstImage = m_nFirstStereoWaveRefImage;
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CDLSComponent::GetInstrumentImageIndex

HRESULT CDLSComponent::GetInstrumentImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*pnFirstImage = m_nFirstInstrumentImage;
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDLSComponent::GetFolderImageIndex

HRESULT CDLSComponent::GetFolderImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*pnFirstImage = m_nFirstFolderImage;
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDLSComponent::GetFolderImageIndex

HRESULT CDLSComponent::GetFolderGrayedImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*pnFirstImage = m_nFirstFolderGrayedImage;
	
	return S_OK;
}


HRESULT CDLSComponent::OpenPort()
{
	HRESULT hr = E_FAIL;

	hr = m_pIFramework->GetSharedObject(CLSID_DirectMusic,IID_IDirectMusic,(void**) &m_pIDirectMusic);
	if (FAILED(hr))
	{
		AfxMessageBox(IDS_ERR_DM_FROM_FRAMEWORK,MB_OK | MB_ICONEXCLAMATION);
		return E_FAIL;
	}

	hr = m_pIConductor->GetPerformanceEngine( (IUnknown**)&m_pIPerformance );
	if (FAILED(hr)) 
	{
		return E_FAIL;
	}


	// Create our own private audio path (if necessary)
	if( m_pIAudioPath == NULL )
	{
		// TODO: Replace with code that duplicates the default performance audio path (Needs API work)
		if( FAILED( m_pIPerformance->CreateStandardAudioPath( DMUS_APATH_DYNAMIC_STEREO, 10, TRUE, &m_pIAudioPath ) ) )
		{
			// Only warn the user if we have any collections open
			if( !m_CollectionList.IsEmpty() )
			{
				AfxMessageBox(IDS_ERR_NO_PORT, MB_OK | MB_ICONEXCLAMATION);
			}

			return S_OK;
		}
	}

	// Now, see what PChannel 0 really maps to
	DWORD dwOutputPChannel;
	hr = m_pIAudioPath->ConvertPChannel( 0, &dwOutputPChannel );
	if( FAILED( hr ) )
	{
		// Only warn the user if we have any collections open
		if( !m_CollectionList.IsEmpty() )
		{
			AfxMessageBox(IDS_ERR_NO_PORT, MB_OK | MB_ICONEXCLAMATION);
		}

		return S_OK;
	}

	// Now, see what synth the audio path is using
	if( FAILED( m_pIPerformance->PChannelInfo( dwOutputPChannel, &m_pISynthPort, NULL, NULL ) ) )
	{
		// Only warn the user if we have any collections open
		if( !m_CollectionList.IsEmpty() )
		{
			AfxMessageBox(IDS_ERR_NO_PORT, MB_OK | MB_ICONEXCLAMATION);
		}

		return S_OK;
	}

	return hr;
}

HRESULT CDLSComponent::GetAppendValueForPort(IDirectMusicPortDownload *pPortDownload, DWORD* pdwAppendValue)
{
	if( pPortDownload == NULL )
		return E_POINTER;

	DWORD dwSamples = 0;
	WAVEFORMATEX waveFormat;
	
	// Fill the memory with invalid values so we can check 
	// if the structure was filled successfully by the synth
	FillMemory(&waveFormat, sizeof(waveFormat), 0xFF);

	DWORD dwFormatSize = sizeof(WAVEFORMATEX);
	DWORD dwBufferSize = 0;
	
	IDirectMusicPort* pIDirecMusicPort = NULL;
	if(FAILED(pPortDownload->QueryInterface(IID_IDirectMusicPort, (void**)&pIDirecMusicPort)))
	{
		return E_FAIL;
	}

	if(FAILED(pPortDownload->GetAppend(&dwSamples)) /*|| 
	   FAILED(pIDirecMusicPort->GetFormat(&waveFormat, &dwFormatSize, &dwBufferSize))||
	   waveFormat.wBitsPerSample == 0xFFFF*/)
	{
		pIDirecMusicPort->Release();
		*pdwAppendValue = 0;

		// When a collection is referenced in a segment this 
		// error message can come up thousands of times so we 
		// keep a flag to check if message this has been shown before
		if(m_bShowGetFormatErrorMessage)
			AfxMessageBox(IDS_ERR_GETFORMAT);
		
		m_bShowGetFormatErrorMessage = false;

		return E_FAIL;
	}
	pIDirecMusicPort->Release();

	// HACK HACK HACK!!! Temporary workaround a DMusic bug!!
	waveFormat.wBitsPerSample = 16;

	// Get the bytes
	*pdwAppendValue = (dwSamples * waveFormat.wBitsPerSample)/8; 

	// Reset the GetFormat error flag
	m_bShowGetFormatErrorMessage = true;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDLSComponent::UpdateDLSPortList

HRESULT CDLSComponent::UpdateDLSPortList( void )
{
	// Initialize hr to an error code.  If we find a DownloadPort, we will set it to S_OK
	HRESULT hr = E_FAIL;

	// Empty the existing m_lstDLSPortDownload
	while( !m_lstDLSPortDownload.IsEmpty() )
	{
		m_lstDLSPortDownload.RemoveHead()->Release();
	}

	bool fAddedDLSSynthPort = false;

	// Ensure m_pIPerformance is set
	ASSERT( m_pIPerformance );
	if( m_pIPerformance )
	{
		// Get a pointer to the default audio path
		IDirectMusicAudioPath *pDMAudioPath;
		if( SUCCEEDED( m_pIPerformance->GetDefaultAudioPath( &pDMAudioPath ) ) )
		{
			// Iterate through the default AudioPath's ports
			IDirectMusicPort *pDMPort;
			DMUS_PORTCAPS dmPortCaps;
			DWORD dwIndex = 0;
			while( S_OK == pDMAudioPath->GetObjectInPath( DMUS_PCHANNEL_ALL, DMUS_PATH_PORT, 0, GUID_All_Objects, dwIndex, IID_IDirectMusicPort, (void **)&pDMPort ) )
			{
				// Verify we have a valid port pointer
				ASSERT( pDMPort );

				// Initialize the DMUS_PORTCAPS structure
				dmPortCaps.dwSize = sizeof(DMUS_PORTCAPS);

				// Try and get the capabilities of this port, and check if it supports DLS
				if( SUCCEEDED( pDMPort->GetCaps( &dmPortCaps ) )
				&&	(dmPortCaps.dwFlags & DMUS_PC_DLS) )
				{
					// Check if we can download to the port
					IDirectMusicPortDownload *pPortDownload = NULL;
					if( SUCCEEDED( pDMPort->QueryInterface( IID_IDirectMusicPortDownload, (void **)&pPortDownload ) ) )
					{
						if( pDMPort == m_pISynthPort )
						{
							fAddedDLSSynthPort = true;
						}

						// Port supports DLS - add it to our list
						m_lstDLSPortDownload.AddHead( pPortDownload );

						// Found a port - set hr to a Success code
						hr = S_OK;
					}
				}

				// Release the port
				pDMPort->Release();

				// Go on to the next port in the audio path
				dwIndex++;
			}

			// Release the Audiopath
			pDMAudioPath->Release();
		}
	}

	if( !fAddedDLSSynthPort )
	{
		// Initialize the DMUS_PORTCAPS structure
		DMUS_PORTCAPS dmPortCaps;
		dmPortCaps.dwSize = sizeof(DMUS_PORTCAPS);

		// Try and get the capabilities of this port, and check if it supports DLS
		if( SUCCEEDED( m_pISynthPort->GetCaps( &dmPortCaps ) )
		&&	(dmPortCaps.dwFlags & DMUS_PC_DLS) )
		{
			// Check if we can download to the port
			IDirectMusicPortDownload *pPortDownload = NULL;
			if( SUCCEEDED( m_pISynthPort->QueryInterface( IID_IDirectMusicPortDownload, (void **)&pPortDownload ) ) )
			{
				// Port supports DLS - add it to our list
				m_lstDLSPortDownload.AddHead( pPortDownload );

				// Found a port - set hr to a Success code
				hr = S_OK;
			}
		}
	}

	return hr;
}

HRESULT CDLSComponent::IsValidPatch(CInstrument* pValidateInstrument, 
									ULONG ulBank, 
									ULONG ulInstrument, 
									CCollection* pLoadingCollection, 
									CInstrument** ppInstrumentCollision)
{
	bool						bValid=true;
	HRESULT						hr = DLS_ERR_FIRST;
	
	*ppInstrumentCollision = NULL;
	
	// First check if is GM/GS
	if ( IsGM(ulBank, ulInstrument) )
	{
		hr |= SC_PATCH_DUP_GM;
	}

	CCollection* pCollection = m_CollectionList.GetHead();
	while (pCollection && bValid)
	{
		CInstrument* pInstrument = pCollection->m_Instruments.GetHead();
		while(pInstrument) // for each instrument
		{	//compare
			if (pInstrument != pValidateInstrument
				&&(pInstrument->m_rInstHeader.Locale.ulBank & 0x80007F7F) == (ulBank & 0x80007F7F)
			    &&(pInstrument->m_rInstHeader.Locale.ulInstrument & 0x0000007F) == (ulInstrument & 0x00007F7F) )
			{
				// If pLoadingCollection was passed in we need to validate across collections
				// so don't set bValid = false just yet.
				if (!pLoadingCollection)
					bValid = false;
				*ppInstrumentCollision = pInstrument;
				if ( pCollection == pLoadingCollection )
				{
					hr |= SC_PATCH_DUP_COLLECTION;
				}
				else
				{
					hr |= SC_PATCH_DUP_SYNTH;
				}
				//to limit the search: once we establish that both types of collision exist we can stop
				if ( (hr & (SC_PATCH_DUP_COLLECTION | SC_PATCH_DUP_SYNTH)) == (SC_PATCH_DUP_COLLECTION | SC_PATCH_DUP_SYNTH) ) 
				{
					bValid = false;
				}
				break;
			}
			pInstrument = pInstrument->GetNext();
		}
		pCollection =  pCollection->GetNext();
	}
	if (DLS_ERR_FIRST == hr)
		hr = S_OK;
	return hr;
}

HRESULT CDLSComponent::GetNextValidPatch(DWORD dwDrum, BYTE* pWMSB, BYTE* pWLSB, BYTE* pWPatch)
{
	ASSERT(pWMSB != NULL);
	ASSERT(pWLSB != NULL);
	ASSERT(pWPatch != NULL);

	UINT	uMSB, uLSB, uPatch;
	int		x, y, z; //counters
	HRESULT		hr;
	CInstrument*				pDummy;
	bool						bFound = false;

	uMSB = *pWMSB;

	for (x = 0; x < 128; x++)
	{
		for (y = 0, uLSB = *pWLSB; y < 128; y++)
		{
			for (z = 0, uPatch = *pWPatch; z < 128; z++)
			{
				hr = IsValidPatch(NULL, MAKE_BANK(dwDrum, uMSB + x, uLSB + y), uPatch + z , NULL, &pDummy);
				if (SUCCEEDED(hr))
				{
					*pWMSB = uMSB + x;
					*pWLSB = uLSB + y;
					*pWPatch = uPatch + z;
					return S_OK;
				}
				else
				{
					if ((uPatch + z) == 127)
					{
						uPatch = (UINT) -z - 1;
					}
				}
			}
			if ((uLSB + y) == 127)
			{
				uLSB = (UINT)  -y - 1;
			}
		}
		if ((uMSB + x) == 127)
		{
			uMSB = (UINT) -x - 1;
		}
	}
	return E_FAIL;
}

bool CDLSComponent::IsGM(DWORD dwBank, DWORD dwInstrument)
{
	BYTE	bMSB, bLSB, bPatch;

	bMSB = (BYTE) ((dwBank & 0x00007F00) >> 8);
	bLSB = (BYTE) (dwBank & 0x0000007F);
	bPatch = (BYTE) (dwInstrument & 0x0000007F);
	
	if (bLSB != 0) return false;

	if ( (bMSB == 0) ) // && (bLSB == 0) 
	{
		if ((dwBank & 0x80000000)) //Drum Kit
		{
			if ((bPatch == 0x0)  ||
				(bPatch == 0x08) ||
				(bPatch == 0x10) ||
				(bPatch == 0x18) ||
				(bPatch == 0x19) ||
				(bPatch == 0x20) ||
				(bPatch == 0x28) ||
				(bPatch == 0x30) || 
				(bPatch == 0x38) )
			{
				return  true;
			}
			else
				return false;
		}
		else return true;//is GM
	}
	// check for GS
	switch (bMSB)
	{
		case 6:
		case 7:
			if (bPatch == 0x7D) return true;
			break;
		case 24:
			if ((bPatch == 0x04) || (bPatch == 0x06)) return true;
			break;
		case 9:
			if ((bPatch == 0x0E) || (bPatch == 0x76) || (bPatch == 0x7D)) return true;
			break;
		case 2:
			if ( (bPatch == 0x66) || (bPatch == 0x78) || ((bPatch > 0x79)&&(bPatch < 0x80) )) return true;
			break;
		case 3:
			if ((bPatch > 0x79) && (bPatch < 0x80)) return true;
			break;
		case 4:
		case 5:
			if ( (bPatch == 0x7A) || ((bPatch > 0x7B)&&(bPatch < 0x7F) )) return true;
			break;
		case 32:
			if ((bPatch == 0x10) ||
				(bPatch == 0x11) ||
				(bPatch == 0x18) ||
				(bPatch == 0x34) ) return true;
			break;
		case 1:
			if ((bPatch == 0x26) ||
				(bPatch == 0x39) ||
				(bPatch == 0x3C) ||
				(bPatch == 0x50) ||
				(bPatch == 0x51) ||
				(bPatch == 0x62) ||
				(bPatch == 0x66) ||
				(bPatch == 0x68) ||
				((bPatch > 0x77) && (bPatch < 0x80))) return true;
				break;
		case 16:
			switch (bPatch)
			{
				case 0x00:
					return true;
					break;
				case 0x04:
					return true;
					break;
				case 0x05:
					return true;
					break;
				case 0x06:
					return true;
					break;
				case 0x10:
					return true;
					break;
				case 0x13:
					return true;
					break;
				case 0x18:
					return true;
					break;
				case 0x19:
					return true;
					break;
				case 0x1C:
					return true;
					break;
				case 0x27:
					return true;
					break;
				case 0x3E:
					return true;
					break;
				case 0x3F:
					return true;
					break;
				default:
					return false;
			}
			break;
		case 8:
			if ((bPatch < 0x07) || ((bPatch == 0x7D)))
			{
				return true;
			}
			else if ((bPatch > 0x3F) && (bPatch < 0x50))
			{
				return false;
			}
			else if ((bPatch > 0x4F) && (bPatch < 0x72)  )
			{
				if ((bPatch == 0x50) || 
					(bPatch == 0x51) ||
					(bPatch == 0x6B))
				{
					return true;
				}
				return false;
			}
			else if ((bPatch > 0x1F) && (bPatch < 0x40))
			{
				if ((bPatch > 0x25) && (bPatch < 0x29) ||
					(bPatch > 0x3C)  ||
					(bPatch == 0x30) || 
					(bPatch == 0x32) )
				{
					return true;
				}
				return false;
			}
			else if ((bPatch > 0x0A) && (bPatch < 0x12) && 
				     (bPatch != 0x0D) && (bPatch != 0x0F))
			{
				return true;
			}
			else if ((bPatch > 0x0F) && (bPatch < 0x20))
			{
				if (bPatch > 0x17)
				{
					return true;
				}
				else if ( (bPatch == 0x13) || (bPatch == 0x15) )
					return true;
				else
					return false;
			}
			break;
		default:
			return false;
	}
	return false;
}

void CDLSComponent::NotifyDummyInstruments()
{
	CCollection* pCollection = m_CollectionList.GetHead();
	while (pCollection)
	{
		CWaveNode* pWaveNode = pCollection->m_Waves.GetHead();
		while(pWaveNode)
		{
			CWave* pWave = pWaveNode->GetWave();
			ASSERT(pWave);
			if (pWave)
			{
				pWave->ValidateDummyPatch();
			}
			pWaveNode = pWaveNode->GetNext();
		}
		pCollection = pCollection->GetNext();
	}
}

HRESULT CDLSComponent::GetNextValidDummyPatch(CInstrument* pDummy, BYTE* pWMSB, BYTE* pWLSB, BYTE* pWPatch)
{
	ASSERT(pWMSB != NULL);
	ASSERT(pWLSB != NULL);
	ASSERT(pWPatch != NULL);

	UINT	uMSB, uLSB, uPatch;
	int		x, y, z; //counters
	HRESULT		hr;
	bool						bFound = false;

	uMSB = m_bDummyMSB;

	for (x = 0; x < 128; x++)
	{
		for (y = 0, uLSB = m_bDummyLSB; y < 128; y++)
		{
			for (z = 0, uPatch = m_bDummyPatch; z < 128; z++)
			{
				hr = IsValidDummyPatch(pDummy, MAKE_BANK(0, uMSB - x, uLSB - y), uPatch - z);
				if (SUCCEEDED(hr))
				{
					m_bDummyMSB = *pWMSB = uMSB - x;
					m_bDummyLSB = *pWLSB = uLSB - y;
					*pWPatch = uPatch - z;
					return S_OK;
				}
				else
				{
					if ((uPatch - z) == 0)
					{
						uPatch = (UINT) 126 + z;
					}
				}
			}
			if ((uLSB + y) == 0)
			{
				uLSB = (UINT)  126 + z;
			}
		}
		if ((uMSB + x) == 0)
		{
			uMSB = (UINT) 126 + z;
		}
	}
	return E_FAIL;
}

HRESULT CDLSComponent::IsValidDummyPatch(CInstrument* pValidateInstrument, ULONG ulBank, ULONG ulInstrument)
{
	CInstrument* pDummy = NULL;
	CWaveNode* pWaveNode = NULL;

	HRESULT hr = IsValidPatch(pValidateInstrument, ulBank, ulInstrument, NULL, &pDummy);
	if (!SUCCEEDED(hr))
	{
		return E_FAIL;
	}

	// Get the mono waves for the independent waves as well
	CPtrList wavesInProject;
	if(FAILED(GetListOfWaves(&wavesInProject)))
	{
		return E_FAIL;
	}

	if(!m_CollectionList.IsEmpty())
	{
		CCollection* pCollection = m_CollectionList.GetHead();
		while (pCollection)
		{
			CPtrList wavesInCollection;
			if(FAILED(pCollection->m_Waves.GetListOfWaves(&wavesInCollection)))
			{
				return E_FAIL;
			}

			// Merge the two lists
			CPtrList waveList;
			waveList.AddTail(&wavesInProject);
			waveList.AddTail(&wavesInCollection);
			
			if(FAILED(ValidateWaveDummyInstruments(&waveList, pValidateInstrument, ulBank, ulInstrument)))
			{
				return E_FAIL;
			}

			waveList.RemoveAll();

			pCollection = pCollection->GetNext();
		}
	}
	else
	{
		if(FAILED(ValidateWaveDummyInstruments(&wavesInProject, pValidateInstrument, ulBank, ulInstrument)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CDLSComponent::ValidateWaveDummyInstruments(CPtrList* pWaveList, CInstrument* pValidateInstrument, ULONG ulBank, ULONG ulInstrument)
{
	ASSERT(pValidateInstrument);
	if(pValidateInstrument == NULL)
	{
		return E_POINTER;
	}

	ASSERT(pWaveList);
	if(pWaveList == NULL)
	{
		return E_POINTER;
	}

	POSITION position = pWaveList->GetHeadPosition();
	while(position)
	{
		CMonoWave* pWave = (CMonoWave*) pWaveList->GetNext(position);
		if(pWave)
		{
			CInstrument* pDummy = pWave->GetDummyInstrument();
			if (pDummy && pDummy != pValidateInstrument)
			{
				if( (pDummy->m_rInstHeader.Locale.ulBank & 0x80007F7F) == (ulBank & 0x80007F7F)
					&& (pDummy->m_rInstHeader.Locale.ulInstrument & 0x0000007F) == (ulInstrument & 0x00007F7F) )
				{
					return E_FAIL;
				}
			}	
		}
	}

	return S_OK;
}


// A version of PlayMIDIEvent that lets you specifiy the PChannel.
// needed so that auditioning stuff with the DLS designer won't interfere
// with the rest of producer.
HRESULT CDLSComponent::PlayMIDIEvent(BYTE bStatus, BYTE bData1, BYTE bData2, DWORD dwTime, bool bIsDrum)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// We're in the process of shutting down and performance has probably gone away...
	// Don't do anything now...
	if(m_bInCleanup)
	{
		return S_OK;
	}

	ASSERT( m_pIPerformance != NULL );
	ASSERT( m_pIAudioPath != NULL );
	if( (m_pIPerformance == NULL)
	||	(m_pIAudioPath == NULL) )
	{
		return E_UNEXPECTED;
	}

	HRESULT hr;
	DMUS_PMSG *pPipelineEvent = NULL;

	DMUS_MIDI_PMSG *pDMMidiEvent = NULL;
	hr = m_pIPerformance->AllocPMsg( sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG**)&pDMMidiEvent );
	if ( FAILED(hr) )
	{
		return hr;
	}
	memset( pDMMidiEvent, 0, sizeof(DMUS_MIDI_PMSG) );

	pDMMidiEvent->dwSize = sizeof(DMUS_MIDI_PMSG);
	pDMMidiEvent->bStatus = bStatus;
	pDMMidiEvent->bByte1 = bData1;
	pDMMidiEvent->bByte2 = bData2;
	if ( dwTime == 0 )
	{
		//pDMMidiEvent->m_rtTime = 0;
	}
	else
	{
		REFERENCE_TIME rtNow;
		m_pIPerformance->GetLatencyTime( &rtNow );
		pDMMidiEvent->rtTime = dwTime * 10000 + rtNow;
	}
	pDMMidiEvent->dwFlags = DMUS_PMSGF_REFTIME;
	pDMMidiEvent->dwPChannel = bIsDrum ? 9 : 0;
	pDMMidiEvent->dwVirtualTrackID = 1;
	pDMMidiEvent->dwType = DMUS_PMSGT_MIDI;
	pPipelineEvent = (DMUS_PMSG*) pDMMidiEvent;

	IDirectMusicGraph* pGraph;
	if( SUCCEEDED( m_pIAudioPath->QueryInterface( IID_IDirectMusicGraph, (void **)&pGraph ) ) )
	{
		pGraph->StampPMsg( pPipelineEvent );
		pGraph->Release();
	}

	return m_pIPerformance->SendPMsg( pPipelineEvent );
}


// Applies the SnapToZero selection for all the waves
void CDLSComponent::SetSnapToZero(BOOL bSnapToZero)
{
	m_bSnapToZero = bSnapToZero;
}


BOOL CDLSComponent::IsSnapToZero()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return m_bSnapToZero;
}


void CDLSComponent::InitSnapToZero()
{
	m_bSnapToZero = TRUE;
	DWORD dwType = 0;
	DWORD dwSize = 0;
	CString sValueName = "SnapToZero";
	ReadFromRegistry((TCHAR*)((LPCSTR)sValueName), &dwType, &dwSize, (BYTE*)&m_bSnapToZero);
}

HRESULT	CDLSComponent::ReadFromRegistry(TCHAR* pszValueName, DWORD* pdwType, DWORD* pdwDataSize, BYTE* pbData, TCHAR* pszSubPath)
{
	ASSERT(pbData);
	if(pbData == NULL)
	{
		return E_FAIL;
	}

	TCHAR szComponentPath[MAX_PATH];
	TCHAR szValueName[MAX_PATH];
	_tcscpy( szComponentPath, _T("Software\\Microsoft\\DMUSProducer\\DLSDesigner\\"));
	if(pszSubPath != NULL)
	{
		_tcscat(szComponentPath, pszSubPath);
	}
	
	HRESULT hr = E_FAIL;
	HKEY	hkeyOpen;
	if(::RegOpenKeyEx(HKEY_CURRENT_USER, szComponentPath, 0, KEY_READ, &hkeyOpen) == ERROR_SUCCESS)
	{
		if(::RegQueryValueEx(hkeyOpen, szValueName, 0, pdwType, pbData, pdwDataSize) == ERROR_SUCCESS)
		{
			hr = S_OK;
		}

		::RegCloseKey(hkeyOpen);
	}

	return hr;
}


void CDLSComponent::InitQueryList()
{
	// Delete all previous queries if there are any...
	while(!m_lstDLSQueries.IsEmpty())
	{
		delete m_lstDLSQueries.RemoveHead();
	}

	TCHAR szComponentPath[MAX_PATH];
	_tcscpy( szComponentPath, _T("Software\\Microsoft\\DMUSProducer\\DLSDesigner\\Queries\\"));

	HKEY	hkeyOpen;
	TCHAR	szGuid[100];
	DWORD	dwSize = sizeof(TCHAR) * 100;
	DWORD	dwIndex = 0;
	TCHAR	szQueryName[MAX_PATH];
	DWORD	dwType = 0;
	DWORD	dwDataSize = MAX_PATH;
	
	if(::RegOpenKeyEx(HKEY_CURRENT_USER, szComponentPath, 0, KEY_READ, &hkeyOpen) == ERROR_SUCCESS)
	{
		while(::RegEnumValue(hkeyOpen, dwIndex, szGuid, &dwSize, NULL, &dwType, (BYTE*)(szQueryName), &dwDataSize) != ERROR_NO_MORE_ITEMS)
		{
			GUID guidQuery;
			WCHAR szWChar[100];
			ZeroMemory( szWChar, 100 * sizeof( WCHAR ) );
			::MultiByteToWideChar( CP_ACP, 0, szGuid, -1, szWChar, 100 );
			if(SUCCEEDED(::IIDFromString(szWChar, &guidQuery)))
			{
				CDLSQuery* pDLSQuery = new CDLSQuery(guidQuery, szQueryName);
				if(pDLSQuery)
				{
					m_lstDLSQueries.AddTail(pDLSQuery);
				}
			}
			
			dwIndex++;
			dwSize = sizeof(TCHAR) * 100;
			dwDataSize = MAX_PATH;
		}

		::RegCloseKey(hkeyOpen);
	}
	
	
}


BOOL CDLSComponent::IsInReferenceLoad()
{
	return m_bInReferencedLoad;
}


void CDLSComponent::AddToFailedCollectionGUIDs(GUID collectionGUID)
{
	// Search to see if we already have this GUID in the list; don't add it again if it does
	if(FindCollectionGUID(collectionGUID) == NULL)
	{
		CCollectionGUIDItem* pGUIDItem = new CCollectionGUIDItem(collectionGUID);
		m_lstFailedCollectionGUIDs.AddTail(pGUIDItem);
	}
}

void CDLSComponent::DeleteFromFailedCollectionGUIDs(GUID collectionGUID)
{
	POSITION position = m_lstFailedCollectionGUIDs.GetHeadPosition();
	while(position)
	{
		POSITION oldPosition = position;
		CCollectionGUIDItem* pGUIDItem = (CCollectionGUIDItem*) m_lstFailedCollectionGUIDs.GetNext(position);
		ASSERT(pGUIDItem);
		if(::IsEqualGUID(pGUIDItem->m_CollectionGUID, collectionGUID))
		{
			m_lstFailedCollectionGUIDs.RemoveAt(oldPosition);
			delete pGUIDItem;
		}
	}
}

CCollectionGUIDItem* CDLSComponent::FindCollectionGUID(GUID searchGUID)
{
	
	POSITION position = m_lstFailedCollectionGUIDs.GetHeadPosition();
	while(position)
	{
		CCollectionGUIDItem* pGUIDItem = (CCollectionGUIDItem*) m_lstFailedCollectionGUIDs.GetNext(position);
		ASSERT(pGUIDItem);
		if(::IsEqualGUID(pGUIDItem->m_CollectionGUID, searchGUID))
			return pGUIDItem;
	}

	return NULL;
}

void CDLSComponent::DeleteGUIDList()
{
	while(!m_lstFailedCollectionGUIDs.IsEmpty())
	{
		CCollectionGUIDItem* pGUIDItem = (CCollectionGUIDItem*) m_lstFailedCollectionGUIDs.RemoveHead();
		if(pGUIDItem)
		{
			delete pGUIDItem;
		}
	}
}

void CDLSComponent::DeleteQueryList()
{
	while(!m_lstDLSQueries.IsEmpty())
	{
		CDLSQuery* pDLSQuery = (CDLSQuery*) m_lstDLSQueries.RemoveHead();
		if(pDLSQuery)
		{
			delete pDLSQuery;
		}
	}
}


void CDLSComponent::DeleteConfigList()
{
	while(!m_lstConfigurations.IsEmpty())
	{
		delete m_lstConfigurations.RemoveHead();
	}
}

void CDLSComponent::GetPortName(IDirectMusicPortDownload* pIDMDownloadPort, CString& sPortName)
{
	sPortName = "Unknown";
	ASSERT(pIDMDownloadPort);
	if(pIDMDownloadPort == NULL)
		return;

	DMUS_PORTCAPS portCaps;
	ZeroMemory(&portCaps, sizeof(DMUS_PORTCAPS));
	portCaps.dwSize = sizeof(DMUS_PORTCAPS);

	IDirectMusicPort* pIDMPort = NULL;
	if(SUCCEEDED(pIDMDownloadPort->QueryInterface(IID_IDirectMusicPort, (void**) &pIDMPort)))
	{
		if(SUCCEEDED(pIDMPort->GetCaps(&portCaps)))
		{
			sPortName = portCaps.wszDescription;
		}
		pIDMPort->Release();
	}
	return;
}

void CDLSComponent::AddToCollectionList(CCollection* pCollection)
{
	ASSERT(pCollection);
	if(pCollection == NULL)
		return;

	// Check first if this node is already in the list
	if(IsInCollectionList(pCollection) == true)
		return;

	// Get the Collection's GUID
	GUID guidCollection;
	pCollection->GetGUID( &guidCollection );

	// Prevent duplicate GUIDs
	GUID guidCollectionList;
	CCollection* pCollectionInList = m_CollectionList.GetHead();
	while(pCollectionInList)
	{
		pCollectionInList->GetGUID( &guidCollectionList );
		if( ::IsEqualGUID( guidCollectionList, guidCollection ) )
		{
			::CoCreateGuid( &guidCollection );
			pCollection->SetGUID( guidCollection );
			break;
		}
		pCollectionInList = pCollectionInList->GetNext();
	}

	// Add it to the list
	pCollection->AddRef();
	m_CollectionList.AddTail(pCollection);
}

void CDLSComponent::DeleteFromCollectionList(CCollection* pCollection)
{
	ASSERT(pCollection);
	if(pCollection == NULL)
		return;

	// Remove this collection from the list if it's in there...
	if(IsInCollectionList(pCollection) == true)
	{
		m_CollectionList.Remove(pCollection);
		pCollection->Release();
	}
}

bool CDLSComponent::IsInCollectionList(CCollection* pCollection)
{
	ASSERT(pCollection);
	if(pCollection == NULL)
		return false;

	CCollection* pCollectionInList = m_CollectionList.GetHead();
	while(pCollectionInList)
	{
		if(pCollectionInList == pCollection)
		{
			return true;
		}

		pCollectionInList = pCollectionInList->GetNext();
	}

	return false;
}

void CDLSComponent::AddToWaveNodesList(CWaveNode* pWaveNode)
{
	ASSERT(pWaveNode);
	if(pWaveNode == NULL)
		return;

	// Check first if this node is already in the list
	if(IsInWaveNodesList(pWaveNode) == true)
		return;

	GUID guid = pWaveNode->GetFileGUID();
	pWaveNode->SetFileGUID(guid);

	// AddRef the node
	pWaveNode->AddRef();

	// Add it to the list
	m_lstWaveNodes.AddTail(pWaveNode);
}


void CDLSComponent::DeleteFromWaveNodesList(CWaveNode* pWaveNode)
{
	ASSERT(pWaveNode);
	if(pWaveNode == NULL)
		return;

	POSITION position = m_lstWaveNodes.GetHeadPosition();
	while(position)
	{
		POSITION nodePosition = position;
		CWaveNode* pWaveNodeInList = (CWaveNode*) m_lstWaveNodes.GetNext(position);
		ASSERT(pWaveNodeInList);
		if(pWaveNodeInList == pWaveNode)
		{
			m_lstWaveNodes.RemoveAt(nodePosition);
			pWaveNode->Release();
			return;
		}
	}

	// We didn't find the node?
	return;
}


bool CDLSComponent::IsInWaveNodesList(CWaveNode* pWaveNode)
{
	ASSERT(pWaveNode);
	if(pWaveNode == NULL)
		return false;

	POSITION position = m_lstWaveNodes.GetHeadPosition();
	while(position)
	{
		CWaveNode* pWaveNodeInList = (CWaveNode*) m_lstWaveNodes.GetNext(position);
		ASSERT(pWaveNodeInList);
		if(pWaveNodeInList == pWaveNode)
		{
			return true;
		}
	}

	return false;
}


bool CDLSComponent::IsInWaveNodesList(GUID guidFile)
{
	POSITION position = m_lstWaveNodes.GetHeadPosition();
	while(position)
	{
		CWaveNode* pWaveNodeInList = (CWaveNode*) m_lstWaveNodes.GetNext(position);
		ASSERT(pWaveNodeInList);
		if(pWaveNodeInList)
		{
			if(::IsEqualGUID(pWaveNodeInList->GetFileGUID(), guidFile))
			{
				return true;
			}
		}
	}

	return false;

}

HRESULT CDLSComponent::GetListOfWaves(CPtrList* pWaveList)
{
	ASSERT(pWaveList);
	if(pWaveList == NULL)
		return E_POINTER;

	POSITION position = m_lstWaveNodes.GetHeadPosition();
	while(position)
	{
		CWaveNode* pWaveNode = (CWaveNode*) m_lstWaveNodes.GetNext(position);
		ASSERT(pWaveNode);
		if(pWaveNode)
		{
			CWave* pWave = pWaveNode->GetWave();
			ASSERT(pWave);
			if(pWave)
			{
				pWaveList->AddTail(pWave);
			}
		}
	}

	return S_OK;
}



BOOL CDLSComponent::IsAPresetQuery(const CString sQueryName, GUID& guidQuery)
{
	POSITION position = m_lstDLSQueries.GetHeadPosition();
	while(position)
	{
		CDLSQuery* pQuery = (CDLSQuery*) m_lstDLSQueries.GetNext(position);
		if(pQuery)
		{
			if(sQueryName.CompareNoCase(pQuery->GetName()) == 0)
			{
				guidQuery = pQuery->GetGUID();
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CDLSComponent::IsAPresetQuery(const GUID guidQuery, CString& sQueryName )
{
	POSITION position = m_lstDLSQueries.GetHeadPosition();
	while(position)
	{
		CDLSQuery* pQuery = (CDLSQuery*) m_lstDLSQueries.GetNext(position);
		if(pQuery)
		{
			if(::IsEqualIID(pQuery->GetGUID(), guidQuery))
			{
				sQueryName = pQuery->GetName();
				return TRUE;
			}
		}
	}

	return FALSE;
}

const CPtrList& CDLSComponent::GetListOfQueries()
{
	return m_lstDLSQueries;
}

CDLSQuery* CDLSComponent::GetDLSQuery(const GUID& guidQuery)
{
	POSITION position = m_lstDLSQueries.GetHeadPosition();
	while(position)
	{
		CDLSQuery* pQuery = (CDLSQuery*) m_lstDLSQueries.GetNext(position);
		if(pQuery)
		{
			if(::IsEqualIID(pQuery->GetGUID(), guidQuery))
			{
				return pQuery;
			}
		}
	}

	return NULL;
}

void CDLSComponent::WriteQueryToRegistryAndAddToList(CDLSQuery* pDLSQuery)
{
	ASSERT(pDLSQuery);
	if(pDLSQuery == NULL)
		return;

	WriteQueryToRegistry(pDLSQuery->GetGUID(), pDLSQuery->GetName());
	m_lstDLSQueries.AddTail(pDLSQuery);
}


void CDLSComponent::RefreshConfigToolbar()
{
	if(m_pConditionConfigToolbar)
	{
		m_pConditionConfigToolbar->RefreshConfigCombo(&m_lstConfigurations);
	}
}

void CDLSComponent::OnConditionConfigChanged(CSystemConfiguration* pCurrentConfig, bool bRefreshNode)
{
	ASSERT(pCurrentConfig);
	if(pCurrentConfig == NULL)
	{
		return;
	}

	CCollection* pCollection = m_CollectionList.GetHead();
	while(pCollection)
	{
		pCollection->OnConditionConfigChanged(pCurrentConfig, bRefreshNode);
		pCollection = pCollection->GetNext();
	}
}

CSystemConfiguration* CDLSComponent::GetCurrentConfig()
{
//	ASSERT(m_pConditionConfigToolbar);
	if(m_pConditionConfigToolbar)
	{
		return m_pConditionConfigToolbar->GetCurrentConfig();
	}

	return NULL;
}



CSystemConfiguration* CDLSComponent::GetConfiguration(CString sConfigName)
{
	// Should never be empty...
	// We should create a configuration called "<None>" which has all values set to 0
	// and it always lives in the list till we die....
	if(m_lstConfigurations.IsEmpty())
	{
		return NULL;
	}

	POSITION position = m_lstConfigurations.GetHeadPosition();
	while(position)
	{
		CSystemConfiguration* pConfig = (CSystemConfiguration*) m_lstConfigurations.GetNext(position);
		ASSERT(pConfig);
		if(pConfig->GetName().CompareNoCase(sConfigName) == 0)
		{
			return pConfig;
		}
	}

	return NULL;
}

void CDLSComponent::DeleteConfiguration(CSystemConfiguration* pConfig)
{
	ASSERT(pConfig);
	if(pConfig == NULL || m_lstConfigurations.IsEmpty())
	{
		return;
	}
	
	POSITION position = m_lstConfigurations.Find(pConfig);
	if(position)
	{
		m_lstConfigurations.RemoveAt(position);
		delete pConfig;
	}
	
	UpdateConfigsInRegistry();
}

void CDLSComponent::AddConfiguration(CSystemConfiguration* pConfig)
{
	ASSERT(pConfig);
	if(pConfig == NULL)
	{
		return;
	}

	m_lstConfigurations.AddTail(pConfig);
	UpdateConfigsInRegistry();
}

void CDLSComponent::WriteConfigToRegistry(CSystemConfiguration* pConfig)
{
	ASSERT(pConfig);
	if(pConfig == NULL)
	{
		return;
	}

	// Create a key with the config name
	CString sSubPath = "Configs\\" + pConfig->GetName() + "\\";

	// Write all the query value pairs
	CPtrList& lstQueryValue = pConfig->GetQueryValueList();
	POSITION position = lstQueryValue.GetHeadPosition();
	while(position)
	{
		CQueryValuePair* pQueryValue = (CQueryValuePair*)lstQueryValue.GetNext(position);
		ASSERT(pQueryValue);
		if(pQueryValue)
		{
			CDLSQuery* pQuery = pQueryValue->GetQuery();
			ASSERT(pQuery);
			DWORD dwValue = pQueryValue->GetValue();
			GUID guidQuery = pQuery->GetGUID();
			LPOLESTR psz;
			if( SUCCEEDED( ::StringFromIID(guidQuery, &psz) ) )
			{
				TCHAR szGuid[100];
				WideCharToMultiByte( CP_ACP, 0, psz, -1, szGuid, sizeof(szGuid), NULL, NULL );
				CoTaskMemFree( psz );

				WriteToRegistry(szGuid, REG_DWORD, (BYTE*)(&dwValue), sizeof(DWORD), sSubPath.GetBuffer(0));
			}
		}
	}
}


void CDLSComponent::UpdateConfigsInRegistry()
{
	TCHAR szConfigsPath[MAX_PATH];
	_tcscpy( szConfigsPath, _T("Software\\Microsoft\\DMUSProducer\\DLSDesigner\\Configs\\"));

	HKEY hkeyOpen;
	if(::RegOpenKeyEx(HKEY_CURRENT_USER, szConfigsPath, 0, KEY_ALL_ACCESS, &hkeyOpen) == ERROR_SUCCESS)
	{
		if(SHDeleteKey(hkeyOpen, 0) != ERROR_SUCCESS)
		{
			::RegCloseKey(hkeyOpen);
			return;
		}
	}

	POSITION position = m_lstConfigurations.GetHeadPosition();
	while(position)
	{
		CSystemConfiguration* pConfig = (CSystemConfiguration*)m_lstConfigurations.GetNext(position);
		ASSERT(pConfig);
		if(pConfig && pConfig->IsDefault() == false)
		{
			WriteConfigToRegistry(pConfig);
		}
	}

	::RegCloseKey(hkeyOpen);
}

void CDLSComponent::LoadConfigurations()
{
	// Can't have configs when no queries defined
	ASSERT(m_lstDLSQueries.IsEmpty() == FALSE);

	// Delete all previous configurations if there are any....
	while(!m_lstConfigurations.IsEmpty())
	{
		delete m_lstConfigurations.RemoveHead();
	}

	if(m_pConditionConfigToolbar)
	{
		m_pConditionConfigToolbar->SetCurrentConfig(NULL);
	}

	TCHAR szComponentPath[MAX_PATH];
	_tcscpy( szComponentPath, _T("Software\\Microsoft\\DMUSProducer\\DLSDesigner\\Configs\\"));

	HKEY	hkeyOpen;

	DWORD	dwKeyIndex = 0;
	DWORD	dwValueIndex = 0;

	TCHAR	szKeyName[MAX_PATH];
	DWORD	dwKeySize = MAX_PATH;
	
	if(::RegOpenKeyEx(HKEY_CURRENT_USER, szComponentPath, 0, KEY_READ, &hkeyOpen) == ERROR_SUCCESS)
	{
		FILETIME fileTime;

		while(::RegEnumKeyEx(hkeyOpen, dwKeyIndex++, szKeyName, &dwKeySize, 0, NULL, NULL, &fileTime) != ERROR_NO_MORE_ITEMS)
		{
			HKEY hkeySubkey;
			if(::RegOpenKeyEx(hkeyOpen, szKeyName, 0, KEY_READ, &hkeySubkey) == ERROR_SUCCESS)
			{
				TCHAR	szGuid[100];
				DWORD	dwSize = sizeof(TCHAR) * 100;
				DWORD	dwValue = 0;
				DWORD	dwValueSize = sizeof(DWORD);
				DWORD	dwType = REG_DWORD;
				DWORD	dwIndex = 0;

				CSystemConfiguration* pConfig = new CSystemConfiguration();
				ASSERT(pConfig);
				if(pConfig == NULL)
				{
					// Out of memory!!
					::RegCloseKey(hkeySubkey);
					::RegCloseKey(hkeyOpen);
					return;
				}

				pConfig->SetName(szKeyName);
				m_lstConfigurations.AddTail(pConfig);

				while(::RegEnumValue(hkeySubkey, dwIndex++, szGuid, &dwSize, NULL, &dwType, (BYTE*)(&dwValue), &dwValueSize) != ERROR_NO_MORE_ITEMS)
				{
					GUID guidQuery;
					WCHAR szWChar[100];
					ZeroMemory(szWChar, 100 * sizeof( WCHAR ));
					::MultiByteToWideChar( CP_ACP, 0, szGuid, -1, szWChar, 100 );

					if(SUCCEEDED(::IIDFromString(szWChar, &guidQuery)))
					{
						CDLSQuery* pQuery = GetDLSQuery(guidQuery);
						if(pQuery)
						{	
							CQueryValuePair* pQueryValue = new CQueryValuePair(pQuery, dwValue);
							ASSERT(pQueryValue);
							if(pQueryValue)
							{
								pConfig->AddQueryValue(pQueryValue);
							}
						}
					}

					dwSize = sizeof(TCHAR) * 100;
					dwValueSize = sizeof(DWORD);
					dwValue = 0;
				}

				dwKeySize = MAX_PATH;
				::RegCloseKey(hkeySubkey);
			}
		}

		::RegCloseKey(hkeyOpen);
	}
}


void CDLSComponent::CreateDefaultConfigs()
{
	// Delete previous configs
	while(!m_lstDefaultConfigs.IsEmpty())
	{
		CSystemConfiguration* pConfig = m_lstDefaultConfigs.RemoveHead();
		ASSERT(pConfig);
		POSITION position = m_lstConfigurations.Find(pConfig);
		if(position)
		{
			m_lstConfigurations.RemoveAt(position);
			delete pConfig;
		}
	}
	
	// Add a None config
	CSystemConfiguration* pConfig = new CSystemConfiguration(true);
	if(pConfig)
	{
		CString sNone;
		sNone.LoadString(IDS_NONE);
		pConfig->SetName(sNone);
		m_lstConfigurations.AddTail(pConfig);
	}


	POSITION position = m_lstDLSPortDownload.GetHeadPosition();
	while(position)
	{
		IDirectMusicPortDownload* pPort = (IDirectMusicPortDownload*)m_lstDLSPortDownload.GetNext(position);
		ASSERT(pPort);
		if(pPort)
		{
			CSystemConfiguration* pConfig = GetConfigForPort(pPort);
			if(pConfig)
			{
				m_lstDefaultConfigs.AddTail(pConfig);
			}
		}
	}

	m_lstConfigurations.AddHead(&m_lstDefaultConfigs);

	// Refresh the toolbar
	RefreshConfigToolbar();
}

CSystemConfiguration* CDLSComponent::GetConfigForPort(IDirectMusicPortDownload* pPort)
{
	ASSERT(pPort);
	if(pPort == NULL)
	{
		return NULL;
	}

	// Query for the IKsControl interface
	IKsControl* pIControl = NULL;
	if(FAILED(pPort->QueryInterface(IID_IKsControl, (void**)&pIControl)))
	{
		return NULL;
	}

	// Get the port name
	IDirectMusicPort* pDMPort = NULL;
	if(FAILED(pPort->QueryInterface(IID_IDirectMusicPort, (void**)&pDMPort)))
	{
		pPort->Release();
		return NULL;
	}

	DMUS_PORTCAPS portCaps;
	ZeroMemory(&portCaps, sizeof(DMUS_PORTCAPS));
	portCaps.dwSize = sizeof(DMUS_PORTCAPS);
	pDMPort->GetCaps(&portCaps);
	CString sPortName = portCaps.wszDescription;

	// Go through all the DLS queries and make a configuration
	CSystemConfiguration* pConfig = new CSystemConfiguration(true);
	pConfig->SetName(sPortName);
	
	POSITION position = m_lstDLSQueries.GetHeadPosition();
	while(position)
	{
		CDLSQuery* pQuery = (CDLSQuery*) m_lstDLSQueries.GetNext(position);
		ASSERT(pQuery);
		if(pQuery)
		{
			DWORD cb = 0;
			DWORD dwFlags = 0;

			KSPROPERTY ksp;
			ksp.Set = pQuery->GetGUID();
			ksp.Id = 0;
			ksp.Flags = KSPROPERTY_TYPE_GET;

			HRESULT hr = pIControl->KsProperty(&ksp, sizeof(ksp), &dwFlags, sizeof(dwFlags), &cb);
			if(SUCCEEDED(hr) || (cb >= sizeof(dwFlags)))
			{
				CQueryValuePair* pQueryValue = new CQueryValuePair(pQuery, (BOOL)dwFlags);
				if(pQueryValue)
				{
					pConfig->AddQueryValue(pQueryValue);
				}
			}
		}
	}

	pDMPort->Release();
	pIControl->Release();

	return pConfig;
}


void CDLSComponent::ExportConfigurations(CString sPath)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString sConfigFileName;
	sConfigFileName.LoadString(IDS_CONFIG_REG);
	
	// Append the name to the path
	sConfigFileName = sPath + "\\" + sConfigFileName;

	TCHAR szComponentPath[MAX_PATH];
	_tcscpy( szComponentPath, _T("Software\\Microsoft\\DMUSProducer\\DLSDesigner\\"));

	HKEY hkeyOpen;
	if(::RegOpenKeyEx(HKEY_CURRENT_USER, szComponentPath, 0, KEY_ALL_ACCESS, &hkeyOpen) == ERROR_SUCCESS)
	{
		HANDLE hProcess = GetCurrentProcess();
		if(hProcess)
		{
			HANDLE hAccessToken;
			if(OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hAccessToken))
			{
				if(SetPrivilege(hAccessToken, SE_BACKUP_NAME, TRUE))
				{
					// Check if the file exists and if it does, delete it so we can save the new one
					DeleteFile(sConfigFileName);

					LONG lResult = ::RegSaveKey(hkeyOpen, sConfigFileName, NULL);
					if(lResult == ERROR_SUCCESS)
					{
						AfxMessageBox(IDS_CONFIG_EXPORT_SUCCESS);
						BSTR bstrFileName = sConfigFileName.AllocSysString();
						ASSERT(m_pIFramework);
						if(m_pIFramework)
						{
							m_pIFramework->ShowFile(bstrFileName);
						}
					}
					else
					{
						CString sErrorMessage;
						sErrorMessage.LoadString(IDS_CONFIG_EXPORT_FAILURE);
						LPVOID lpvError;
						FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM , NULL, lResult, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpvError, 0, NULL);
						CString sDetails;
						sDetails.Format("\n%s", (LPCSTR)lpvError);
						sErrorMessage += sDetails;

						AfxMessageBox(sErrorMessage);
					}

					SetPrivilege(hAccessToken, SE_BACKUP_NAME, FALSE);
				}
			}

			TRACE("OpenThreadToken Failed...error: %u\n", GetLastError()); 
		}

		::RegCloseKey(hkeyOpen);
	}
	else
	{
		AfxMessageBox(IDS_CONFIG_EXPORT_FAILURE);
	}
}


void CDLSComponent::ImportConfigurations(CString sPath)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString sConfigFileName;
	sConfigFileName.LoadString(IDS_CONFIG_REG);
	
	// Append the name to the path
	sConfigFileName = sPath + "\\" + sConfigFileName;

	TCHAR szComponentPath[MAX_PATH];
	_tcscpy( szComponentPath, _T("Software\\Microsoft\\DMUSProducer\\DLSDesigner\\"));

	HKEY	hkeyOpen;
	LPDWORD	lpdwDisposition = NULL;
	if(::RegCreateKeyEx(HKEY_CURRENT_USER, szComponentPath, 0, 0, REG_OPTION_NON_VOLATILE, 
						KEY_ALL_ACCESS, NULL, &hkeyOpen, lpdwDisposition) == ERROR_SUCCESS)
	{
		HANDLE hProcess = GetCurrentProcess();
		if(hProcess)
		{
			HANDLE hAccessToken;
			if(OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hAccessToken))
			{
				if(SetPrivilege(hAccessToken, SE_RESTORE_NAME, TRUE))
				{
					LONG lResult = ::RegRestoreKey(hkeyOpen, sConfigFileName, NULL);
					
					if(lResult == ERROR_SUCCESS)
					{
						// Update the queries
						InitQueryList();

						// Update the configurations
						LoadConfigurations();

						// Recreate the default configs for ports
						CreateDefaultConfigs();

						// Set the first config as selected
						CSystemConfiguration* pConfig = (CSystemConfiguration*)m_lstConfigurations.GetHead();
						m_pConditionConfigToolbar->SetCurrentConfig(pConfig);

						// Refresh the toolbar
						RefreshConfigToolbar();

						AfxMessageBox(IDS_CONFIG_IMPORT_SUCCESS);
					}
					else
					{
						CString sErrorMessage;
						sErrorMessage.LoadString(IDS_CONFIG_IMPORT_FAILURE);
						LPVOID lpvError;
						FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM , NULL, lResult, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpvError, 0, NULL);
						CString sDetails;
						sDetails.Format("\n%s", (LPCSTR)lpvError);
						sErrorMessage += sDetails;

						AfxMessageBox(sErrorMessage);
					}

					SetPrivilege(hAccessToken, SE_RESTORE_NAME, FALSE);
				}
			}
		}
		
		::RegCloseKey(hkeyOpen);
	}
	else
	{
		AfxMessageBox(IDS_CONFIG_IMPORT_FAILURE);
	}

}


BOOL CDLSComponent::SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege) 
{
	LUID luid;
	if(!LookupPrivilegeValue(NULL, lpszPrivilege, &luid)) 
	{      
		TRACE("LookupPrivilegeValue error: %u\n", GetLastError()); 
		return FALSE; 
	}

	TOKEN_PRIVILEGES tp;
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	
	if(bEnablePrivilege)
	{
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	}
	else
	{
		tp.Privileges[0].Attributes = 0;
	}

	// Enable the privilege or disable all privileges.
	AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES) NULL, (PDWORD) NULL);
 
	// Call GetLastError to determine whether the function succeeded.
	if(GetLastError() != ERROR_SUCCESS) 
	{ 
		TRACE("AdjustTokenPrivileges failed: %u\n", GetLastError()); 
		return FALSE; 
	} 

	return TRUE;
}

HRESULT CDLSComponent::UpdateDLSDownloads()
{
	UnloadAllCollections();
	UnloadAllWaves();
	if(m_bAutoDownloadDLS)
	{
		DownloadAllWaves();
		DownloadAllCollections();
	}

	return S_OK;
}


void CDLSComponent::UnloadAllCollections()
{
	ASSERT(m_pIFramework);
	if(m_pIFramework == NULL)
	{
		return;
	}

	CCollection* pCollection = m_CollectionList.GetHead();
	while(pCollection)
	{
		pCollection->Unload();
		pCollection->ReleaseDMCollection();

		IDMUSProdNode* pIDocRootNode = NULL;
		if(FAILED(pCollection->GetDocRootNode(&pIDocRootNode)))
		{
			return;
		}

		m_pIFramework->NotifyNodes(pIDocRootNode, GUID_DMCollectionResync, NULL);
		pIDocRootNode->Release();
		
		pCollection = pCollection->GetNext();
	}
}

void CDLSComponent::DownloadAllCollections()
{
	CCollection* pCollection = m_CollectionList.GetHead();
	while(pCollection)
	{
		pCollection->Download();
		pCollection = pCollection->GetNext();
	}

}

void CDLSComponent::UnloadAllWaves()
{
	POSITION position = m_lstWaveNodes.GetHeadPosition();
	while(position)
	{
		CWaveNode* pWaveNode = (CWaveNode*) m_lstWaveNodes.GetNext(position);
		ASSERT(pWaveNode);
		if(pWaveNode)
		{
			pWaveNode->UnloadFromAllPorts();
		}
	}
}

void CDLSComponent::DownloadAllWaves()
{
	POSITION position = m_lstWaveNodes.GetHeadPosition();
	while(position)
	{
		CWaveNode* pWaveNode = (CWaveNode*) m_lstWaveNodes.GetNext(position);
		ASSERT(pWaveNode);
		if(pWaveNode)
		{
			pWaveNode->InitAndDownload();
		}
	}
}

BOOL CDLSComponent::IsAutoDownloadOkay()
{
	return m_bAutoDownloadDLS;
}

BOOL CDLSComponent::IsCollectionDownloadOK()
{
	return m_bDownloadCollection;
}


void CDLSComponent::SetCollectionPatchConflict(bool bShow)
{
	m_bNoShowCollectionPatchConflict = bShow;
}

bool CDLSComponent::IsNoShowCollectionPatchConflict()
{
	return m_bNoShowCollectionPatchConflict;
}

void CDLSComponent::SetUserPatchConflict(bool bShow)
{
	m_bNoShowUserPatchConflict = bShow;
}

bool CDLSComponent::IsNoShowUserPatchConflict()
{
	return m_bNoShowUserPatchConflict;
}

#ifdef DMP_XBOX
void CDLSComponent::SetBadXboxLoopWarning(bool bShow)
{
	m_bNoShowBadXboxLoopWarning = bShow;
}

bool CDLSComponent::IsNoShowBadXboxLoopWarning()
{
	return m_bNoShowBadXboxLoopWarning;
}
#endif // DMP_XBOX

BOOL CDLSComponent::IsLongLoopsOK()
{
	return m_bLongLoopOK;
}

void CDLSComponent::SetLongLoopOK(BOOL bLongLoopOK)
{
	m_bLongLoopOK = bLongLoopOK;
}


void CDLSComponent::AddToFailedCodecs(DWORD dwCodec)
{
	if(IsInFailedCodecs(dwCodec) == FALSE)
	{
		m_lstFailedCodecs.AddTail(dwCodec);
	}
}

BOOL CDLSComponent::IsInFailedCodecs(DWORD dwCodec)
{
	POSITION position = m_lstFailedCodecs.GetHeadPosition();
	while(position)
	{
		DWORD dwFailedCodec = m_lstFailedCodecs.GetNext(position);
		if(dwFailedCodec == dwCodec)
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CDLSComponent::RemoveFromFailedCodecs(DWORD dwCodec)
{
	if(IsInFailedCodecs(dwCodec))
	{
		POSITION position = m_lstFailedCodecs.Find(dwCodec);
		m_lstFailedCodecs.RemoveAt(position);
	}
}

void CDLSComponent::ClearFailedCodecs()
{
	while(m_lstFailedCodecs.IsEmpty() == FALSE)
	{
		m_lstFailedCodecs.RemoveHead();
	}
}