// Band.cpp : implementation file
//

#include "stdafx.h"

#include "BandEditorDLL.h"
#include "StyleDesigner.h"
#include "Band.h"
#include "BandCtl.h"
#include "DLSDesigner.h"
#include "IMABand.h"
#include <mmreg.h>
#include "Info.h"
#include "PChannelPropPage.h"
#include "PChannelPropPageMgr.h"
#include "PChannelName.h"
#include "resourcemaps.h"
#include "DlgDLS.h"
#include "BandDlg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BInstr abiGS[];
extern BInstr abiDrums[];


static BYTE sabPan[NBR_IMA_INSTRUMENTS]    = {  5, 20, 35, 50, 65, 80, 95, 110,  5, 20, 35, 50, 65, 80, 95, 110 };
static BYTE sabVolume[NBR_IMA_INSTRUMENTS] = { 85, 85, 85, 85, 85, 85, 85,  85, 70, 70, 70, 70, 70, 70, 70,  70 };


// {5574FDE0-0C3B-11d2-89B3-00C04FD912C8}
static const GUID GUID_BandPropPageManager = 
{ 0x5574fde0, 0xc3b, 0x11d2, { 0x89, 0xb3, 0x0, 0xc0, 0x4f, 0xd9, 0x12, 0xc8 } };


CCommonInstrumentPropertyObject CInstrumentListItem::m_CommonPropertyObject;
static const TCHAR achUnknown[16] = "---------------";
long CBand::m_slResourceRefCount = 0;

/////////////////////////////////////////////////////////////////////////////
// CContextMenuHandler

CContextMenuHandler::CContextMenuHandler()
{
	m_pBand = NULL;
}

CContextMenuHandler::~CContextMenuHandler()
{
}


BEGIN_MESSAGE_MAP(CContextMenuHandler, CWnd)
	//{{AFX_MSG_MAP(CContextMenuHandler)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CContextMenuHandler message handlers

BOOL CContextMenuHandler::OnCommand( WPARAM wParam, LPARAM lParam ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = S_FALSE;

	if( m_pBand != NULL )
	{
		hr = m_pBand->OnContextCommand( wParam, lParam );
	}
	
	if( hr == S_OK )
	{
		return TRUE;
	}

	return CWnd::OnCommand( wParam, lParam );
}

//////////////////////////////////////////////////////////////////////
// CLinkedDLSCollection Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLinkedDLSCollection::CLinkedDLSCollection( IDMUSProdNode* pIDocRootNode )
{
    ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
	m_pIDocRootNode->AddRef();

	m_dwUseCount = 1;
}


CLinkedDLSCollection::~CLinkedDLSCollection()
{
	RELEASE( m_pIDocRootNode );
}


//////////////////////////////////////////////////////////////////////
// CBandPropPageManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBandPropPageManager::CBandPropPageManager()
{
	m_pTabBand = NULL;
	m_GUIDManager = GUID_BandPropPageManager;
}

CBandPropPageManager::~CBandPropPageManager()
{
	if( m_pTabBand )
	{
		delete m_pTabBand;
	}

}


/////////////////////////////////////////////////////////////////////////////
// CBandPropPageManager::RemoveCurrentObject

void CBandPropPageManager::RemoveCurrentObject( void )
{
	if( m_pIPropPageObject == NULL )
	{
		return;
	}

	m_pIPropPageObject->OnRemoveFromPageManager();
	m_pIPropPageObject = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CBandPropPageManager IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CBandPropPageManager::GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strTitle;
	CString strDocRootName;
	CString strMotifName;

	// Load "BAND" text
	strTitle.LoadString( IDS_BAND_TEXT );

	CBand* pBand;

	if( m_pIPropPageObject
	&& (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pBand))) )
	{
		// Get DocRoot's Name
		if( pBand->m_pIDocRootNode
		&&  pBand->m_pIDocRootNode != (IDMUSProdNode *)pBand )
		{
			BSTR bstrDocRootName;

			if( SUCCEEDED ( pBand->m_pIDocRootNode->GetNodeName( &bstrDocRootName ) ) )
			{
				strDocRootName = bstrDocRootName;
				::SysFreeString( bstrDocRootName );
			}
		}

		// If DocRoot is GUID_StyleNode.....
		if( pBand->m_pIDocRootNode
		&&  pBand->m_pIDocRootNode != (IDMUSProdNode *)pBand )
		{
			GUID guidNodeId;

			if( strDocRootName.IsEmpty() == FALSE )
			{
				strMotifName = _T(" - ");
			}

			if( SUCCEEDED ( pBand->m_pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
			{
				if( ::IsEqualGUID ( guidNodeId, GUID_StyleNode )  )
				{
					// and ParentNode is GUID_MotifNode.....
					if( pBand->m_pIParentNode )
					{
						if( SUCCEEDED ( pBand->m_pIParentNode->GetNodeId ( &guidNodeId ) ) )
						{
							if( ::IsEqualGUID ( guidNodeId, GUID_MotifNode )  )
							{
								// Get Motif's Name
								BSTR bstrMotifName;

								if( SUCCEEDED ( pBand->m_pIParentNode->GetNodeName( &bstrMotifName ) ) )
								{
									CString strName;
									AfxFormatString1( strName, IDS_MOTIF_TEXT, CString(bstrMotifName) );
									::SysFreeString( bstrMotifName );

									strMotifName = _T(" (");
									strMotifName += strName;
									strMotifName += _T(") - ");
								}
							}
						}
					}
				}
			}
		}

		// Format title
		strTitle += (_T(": ") + strDocRootName + strMotifName + pBand->m_csName);
	}

    *pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandPropPageManager IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CBandPropPageManager::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (hPropSheetPage == NULL)
	||  (pnNbrPages == NULL) )
	{
		return E_POINTER;
	}

	if( pIPropSheet == NULL )
	{
		return E_INVALIDARG;
	}

	m_pIPropSheet = pIPropSheet;
	m_pIPropSheet->AddRef();

	hPropSheetPage[0] = NULL;
	*pnNbrPages = 0;

	// Add Band tab
	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	m_pTabBand = new CTabBand( this );
	if( m_pTabBand )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabBand->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandPropPageManager IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CBandPropPageManager::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDllBasePropPageManager::OnRemoveFromPropertySheet();

	theApp.m_pIPageManager = NULL;

	Release();	// delete myself

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandPropPageManager IDMUSProdPropPageManager::RefreshData

HRESULT CBandPropPageManager::RefreshData( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CBand* pBand;
	
	if( m_pIPropPageObject == NULL )
	{
		pBand = NULL;
	}
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pBand ) ) )
	{
		return E_FAIL;
	}

	// Make sure changes to current Band are processed in OnKillFocus
	// messages before setting the new Band
	CWnd* pWndHadFocus = CWnd::GetFocus();
	CWnd* pWnd = pWndHadFocus;
	CWnd* pWndParent = m_pTabBand->GetParent();

	while( pWnd )
	{
		if( pWnd == pWndParent )
		{
			::SetFocus( NULL );
			break;
		}
		pWnd = pWnd->GetParent();
	}

	// Set Property tabs to display the new Band
	m_pTabBand->SetBand( pBand );

	// Restore focus
	if( pWndHadFocus
	&&  pWndHadFocus != CWnd::GetFocus() )
	{
		pWndHadFocus->SetFocus();
	}

	if(m_pIPropSheet)
		m_pIPropSheet->RefreshTitle();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CBand constructor/destructor

CBand::CBand( CBandComponent* pComponent )
{
	ASSERT( pComponent != NULL );

	m_bInUndoRedo = false;
    m_dwRef = 0;
	AddRef();

	m_pComponent = pComponent;
	m_pComponent->AddRef();

	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;
	m_hWndEditor = NULL;
	m_pAudiopath = NULL;

	InterlockedIncrement( &m_slResourceRefCount );
	if( m_slResourceRefCount == 1 )  
	{
		BInstr   *pbi;

		// allocate and load patch names
		pbi = &abiGS[0];
		while( pbi->nStringId != IDS_PGMEND )
		{
			if( pbi->pstrName == NULL )
			{
				pbi->pstrName = new CString;
				ASSERT( pbi->pstrName != NULL );
				pbi->pstrName->LoadString( pbi->nStringId );
			}
			pbi++;
		}

		// allocate and load drum names
		pbi = &abiDrums[0];
		while( pbi->nStringId != IDS_PGMEND )
		{
			if( pbi->pstrName == NULL )
			{
				pbi->pstrName = new CString;
				ASSERT( pbi->pstrName != NULL );
				pbi->pstrName->LoadString( pbi->nStringId );
			}
			pbi++;
		}

		CPChannelPropPage::CreatePatchPopupMenu();
	}

// Band data for Music Engine
	TCHAR achName[SMALL_BUFFER];
	short nOne = 1;

	::LoadString( theApp.m_hInstance, IDS_BAND_TEXT, achName, SMALL_BUFFER );
    m_csName.Format( "%s%d", achName, nOne );

	InitInstrumentList();

	m_pBandCtrl = NULL;
	m_fDefault = FALSE;
	::CoCreateGuid( &m_guidBand );
	m_fDirty = FALSE;

	m_pDMInstrumentForContextMenu = NULL;
	m_dwPChannelForContextMenu = 0;
	RECT rect = {0,0,0,0};
	m_ContextMenuHandler.CreateEx(0L, NULL, "Hidden band editor menu handler",
								  WS_POPUP | WS_CHILD, rect, NULL, 0, NULL);
	m_ContextMenuHandler.m_pBand = this;
}

CBand::~CBand()
{
	ASSERT( theApp.m_pIFramework != NULL );


	RELEASE( m_pAudiopath );

	DeleteInstrumentList();
	
	// DLSCollection list should be empty!
	ASSERT( m_lstDLSCollections.IsEmpty() );

	// Remove CBand from clipboard
	theApp.FlushClipboard( this );

	// Remove CBand from property sheet
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RemovePageManagerByObject( this );
		pIPropSheet->Release();
	}

	if( InterlockedDecrement( &m_slResourceRefCount ) == 0 )  // last instance
	{
		BInstr   *pbi;

		// destroy the patch names
		pbi = &abiGS[0];
		while( pbi->nStringId != IDS_PGMEND )
		{
			if( pbi->pstrName )  
			{
				delete pbi->pstrName;
				pbi->pstrName = NULL;
			}
			pbi++;
		}

		// destroy the drums names
		pbi = &abiDrums[0];
		while( pbi->nStringId != IDS_PGMEND )
		{
			if( pbi->pstrName )  
			{
				delete pbi->pstrName;
				pbi->pstrName = NULL;
			}
			pbi++;
		}	

		::DestroyMenu(CPChannelPropPage::m_hmenuPatch);
		::DestroyMenu(CPChannelPropPage::m_hmenuDrums);
	
	}

	if(IsWindow(m_ContextMenuHandler.m_hWnd))
	{
		m_ContextMenuHandler.m_pBand = NULL;
		//m_ContextMenuHandler.DestroyWindow();
		::DestroyWindow( m_ContextMenuHandler.m_hWnd );
	}

	if( m_pComponent )
	{
		m_pComponent->Release();
	}
}

void CBand::InitInstrumentList()
{
	for( int nCount = 0 ; nCount < NBR_IMA_INSTRUMENTS ; ++nCount )
    {
		CDMInstrument* pInstrument = new CDMInstrument();	
        
		pInstrument->dwPatch = 0;
		pInstrument->dwAssignPatch = 0;
		pInstrument->dwPChannel = nCount;
		pInstrument->dwPatch = MAKE_PATCH(pInstrument->IsDrums(), 0, 0, 0);
		pInstrument->dwFlags = (DMUS_IO_INST_GS | DMUS_IO_INST_GM);
		memset(pInstrument->dwNoteRanges,0,4 * sizeof(DWORD));
		pInstrument->bPan = sabPan[nCount];
		pInstrument->bVolume = sabVolume[nCount];
		pInstrument->nTranspose = 0;
		pInstrument->nPitchBendRange = 2;
		pInstrument->dwChannelPriority = dwaPriorities[nCount];
		
		pInstrument->SetComponent(m_pComponent);
		pInstrument->SetBand(this);


		CInstrumentListItem* pInstrumentListItem = new CInstrumentListItem(pInstrument);
		m_lstInstruments.AddTail(pInstrumentListItem);
    }
}


void CBand::DeleteInstrumentList()
{
	CInstrumentListItem* pInstrumentItem;

	while( !m_lstInstruments.IsEmpty() )
	{
		pInstrumentItem = m_lstInstruments.RemoveHead();
		//delete pInstrumentItem;
		if(pInstrumentItem->Release() == 0)
			pInstrumentItem = NULL;
	}
}

void* CBand::GetInstrumentList()
{
	return &m_lstInstruments;
}


/////////////////////////////////////////////////////////////////////////////
// CBand::SetModifiedFlag

void CBand::SetModifiedFlag( BOOL fNeedsSaved )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	// Framework responsible for clearing its own modified flag.
	// Otherwise Project file containing multiple dirty files
	// may be flagged as not dirty because one file is no 
	// longer dirty.
	if( fNeedsSaved )
	{
		m_fDirty = TRUE;
	}
    else
    {
        m_fDirty = FALSE;
    }
}


/////////////////////////////////////////////////////////////////////////////
// CBand IUnknown implementation

HRESULT CBand::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdNode)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        *ppvObj = (IDMUSProdNode *)this;
    }

    else if( ::IsEqualIID(riid, IID_IDMUSProdBandEdit) )
    {
        *ppvObj = (IDMUSProdBandEdit *)this;
    }

	else if( ::IsEqualIID(riid, IID_IDMUSProdBandEdit8) )
	{
		*ppvObj = (IDMUSProdBandEdit8 *)this;
	}

	else if( ::IsEqualIID(riid, IID_IDMUSProdBandEdit8a) )
	{
		*ppvObj = (IDMUSProdBandEdit8a *)this;
	}

	else if( ::IsEqualIID(riid, IID_IDMUSProdBandMgrEdit) )
	{
		*ppvObj = (IDMUSProdBandMgrEdit *)this;
	}

    else if( ::IsEqualIID(riid, IID_IDMUSProdNotifySink) )
    {
        *ppvObj = (IDMUSProdNotifySink *)this;
    }

	else if( ::IsEqualIID(riid, IID_IDMUSProdPropPageObject) )
	{
		*ppvObj = (IDMUSProdPropPageObject *)this;
	}

    else if( ::IsEqualIID(riid, IID_IPersist) )
    {
        *ppvObj = (IPersist *)this;
    }

    else if( ::IsEqualIID(riid, IID_IPersistStream) )
    {
        *ppvObj = (IPersistStream *)this;
    }

    else if( ::IsEqualIID(riid, IID_IDMUSProdGetReferencedNodes) )
    {
        *ppvObj = (IDMUSProdGetReferencedNodes *)this;
    }
	else
	{
	    *ppvObj = NULL;
		return E_NOINTERFACE;
	}
	
	AddRef();
	return S_OK;
}

ULONG CBand::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CBand::Release()
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
// CBand IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::GetNodeImageIndex

HRESULT CBand::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );

	return( m_pComponent->GetBandImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::GetFirstChild

HRESULT CBand::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return E_NOTIMPL;	// No children for Band nodes
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::GetNextChild

HRESULT CBand::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return E_NOTIMPL;	// No children for Band nodes
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::GetNodeListInfo

HRESULT CBand::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Only applies to band 'files'
	if( m_pIDocRootNode == this )
	{
		CString strDescriptor;

		pListInfo->bstrName = m_csName.AllocSysString();
		pListInfo->bstrDescriptor = strDescriptor.AllocSysString();
		memcpy( &pListInfo->guidObject, &m_guidBand, sizeof(GUID) );

		// Must check pListInfo->wSize before populating additional fields
		return S_OK;
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::GetComponent

HRESULT CBand::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );

	return m_pComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::GetDocRootNode

HRESULT CBand::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pIDocRootNode )
	{
		m_pIDocRootNode->AddRef();
		*ppIDocRootNode = m_pIDocRootNode;
		return S_OK;
	}

	*ppIDocRootNode = NULL;
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::SetDocRootNode

HRESULT CBand::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::GetParentNode

HRESULT CBand::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIParentNode == NULL )
	{
		return E_POINTER;
	}

	*ppIParentNode = m_pIParentNode;

	if( m_pIParentNode )
	{
		m_pIParentNode->AddRef();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::SetParentNode

HRESULT CBand::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::GetNodeId

HRESULT CBand::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_BandNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::GetNodeName

HRESULT CBand::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    *pbstrName = m_csName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::GetNodeNameMaxLength

HRESULT CBand::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = DMUS_MAX_NAME;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::ValidateNodeName

HRESULT CBand::ValidateNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = S_OK;

	CString strName = bstrName;
	::SysFreeString( bstrName );

	if( m_pIDocRootNode
	&&  m_pIDocRootNode != this )
	{
		// Store original Band name
		CString strOrigName = m_csName;

		m_csName = strName;

		// Give DocRoot node a chance to validate the new Band name
		IDMUSProdNotifySink* pINotifySink;

		if( SUCCEEDED ( m_pIDocRootNode->QueryInterface( IID_IDMUSProdNotifySink, (void **)&pINotifySink ) ) )
		{
			hr = pINotifySink->OnUpdate( this, BAND_ValidateNameChange, NULL  );
			pINotifySink->Release();
		}

		// Put back original Band name
		m_csName = strOrigName;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::SetNodeName

HRESULT CBand::SetNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName = bstrName;
	::SysFreeString( bstrName );

	m_csName = strName;
	SetModifiedFlag( TRUE );

	if( m_pIDocRootNode
	&&  m_pIDocRootNode != this )
	{
		// Notify Style DocRoot node that Band name has changed
		IDMUSProdNotifySink* pINotifySink;

		if( SUCCEEDED ( m_pIDocRootNode->QueryInterface( IID_IDMUSProdNotifySink, (void **)&pINotifySink ) ) )
		{
			pINotifySink->OnUpdate( this, BAND_NameChange, NULL  );
			pINotifySink->Release();
		}
	}
	else
	{
		// Notify connected nodes that Band name has changed
		m_pComponent->m_pIFramework->NotifyNodes( this, BAND_NameChange, NULL );
	}

	CBandPropPageManager* pPageManager = NULL;
	// Get the Band page manager
	if( theApp.m_pIPageManager && theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_BandPropPageManager ) == S_OK )
	{
		pPageManager = (CBandPropPageManager*) theApp.m_pIPageManager;
		pPageManager->RefreshData();
	}


	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::GetEditorClsId

HRESULT CBand::GetEditorClsId( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    *pClsId = CLSID_BandEditor;
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::GetEditorTitle

HRESULT CBand::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pbstrTitle == NULL )
	{
		return E_POINTER;
	}

	// Load "BAND" text
	CString strBand;
	strBand.LoadString( IDS_BAND_TEXT );

	// Get DocRoot's Name
	CString strDocRootName;
	if( m_pIDocRootNode
	&&  m_pIDocRootNode != (IDMUSProdNode *)this )
	{
		BSTR bstrDocRootName;

		if( SUCCEEDED ( m_pIDocRootNode->GetNodeName( &bstrDocRootName ) ) )
		{
			strDocRootName = bstrDocRootName;
			::SysFreeString( bstrDocRootName );
		}
	}

	// If DocRoot is GUID_StyleNode.....
	CString strMotifName;
	if( m_pIDocRootNode
	&&  m_pIDocRootNode != (IDMUSProdNode *)this )
	{
		GUID guidNodeId;

		if( strDocRootName.IsEmpty() == FALSE )
		{
			strMotifName = _T(" - ");
		}

		if( SUCCEEDED ( m_pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
		{
			if( ::IsEqualGUID ( guidNodeId, GUID_StyleNode )  )
			{
				// and ParentNode is GUID_MotifNode.....
				if( m_pIParentNode )
				{
					if( SUCCEEDED ( m_pIParentNode->GetNodeId ( &guidNodeId ) ) )
					{
						if( ::IsEqualGUID ( guidNodeId, GUID_MotifNode )  )
						{
							// Get Motif's Name
							BSTR bstrMotifName;

							if( SUCCEEDED ( m_pIParentNode->GetNodeName( &bstrMotifName ) ) )
							{
								CString strName;
								AfxFormatString1( strName, IDS_MOTIF_TEXT, CString(bstrMotifName) );
								::SysFreeString( bstrMotifName );

								strMotifName = _T(" (");
								strMotifName += strName;
								strMotifName += _T(") - ");
							}
						}
					}
				}
			}
		}
	}

	// Format title
	CString strTitle;
	strTitle = strBand + _T(": ") + strDocRootName + strMotifName + m_csName;

    *pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::GetEditorWindow

HRESULT CBand::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*hWndEditor = m_hWndEditor;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::SetEditorWindow

HRESULT CBand::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_hWndEditor = hWndEditor;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::UseOpenCloseImages

HRESULT CBand::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::GetRightClickMenuId

HRESULT CBand::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance = theApp.m_hInstance;

	if(	m_pIDocRootNode == this )
	{
		*pnMenuId = IDM_BAND_DOCROOT_RMENU;
	}
	else
	{
		*pnMenuId = IDM_BAND_NODE_RMENU;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::OnRightClickMenuInit

HRESULT CBand::OnRightClickMenuInit( HMENU hMenu )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CMenu menu;
	CString strMenuText;

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	IDMUSProdNode* pIThisNode = (IDMUSProdNode *)this;
	IDMUSProdNode* pIDocRootNode;

	if( !SUCCEEDED ( GetDocRootNode(&pIDocRootNode) ) )
	{
		return E_FAIL;
	}

	if( menu.Attach(hMenu) )
	{
		TCHAR achMenuText[MID_BUFFER];

		menu.EnableMenuItem( IDM_RENAME, (MF_ENABLED | MF_BYCOMMAND) );

		if( pIThisNode != pIDocRootNode )
		{
			// IDM_CUT
			if( CanCut() == S_OK )
			{
				menu.EnableMenuItem( IDM_CUT, (MF_ENABLED | MF_BYCOMMAND) );
			}
			else
			{
				menu.EnableMenuItem( IDM_CUT, (MF_GRAYED | MF_BYCOMMAND) );
			}

			// IDM_COPY
			if( CanCopy() == S_OK )
			{
				menu.EnableMenuItem( IDM_COPY, (MF_ENABLED | MF_BYCOMMAND) );
			}
			else
			{
				menu.EnableMenuItem( IDM_COPY, (MF_GRAYED | MF_BYCOMMAND) );
			}

			// IDM_PASTE
			{
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
			}

			// IDM_DELETE
			if( CanDelete() == S_OK )
			{
				menu.EnableMenuItem( IDM_DELETE, (MF_ENABLED | MF_BYCOMMAND) );
			}
			else
			{
				menu.EnableMenuItem( IDM_DELETE, (MF_GRAYED | MF_BYCOMMAND) );
			}

			// IDM_RENAME
			if( m_pIParentNode == NULL )
			{
				menu.EnableMenuItem( IDM_RENAME, (MF_GRAYED | MF_BYCOMMAND) );
			}

			// IDM_CREATE_FILE
			menu.InsertMenu( 0, MF_BYPOSITION | MF_SEPARATOR );
			::LoadString( theApp.m_hInstance, IDS_SAVE_AS_TEXT, achMenuText, MID_BUFFER );
			menu.InsertMenu( 0, MF_BYPOSITION | MF_STRING, IDM_CREATE_FILE, achMenuText );
		}

		menu.Detach();
	}

	pIDocRootNode->Release();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::OnRightClickMenuSelect

HRESULT CBand::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	HRESULT hr = E_FAIL;

	IDataObject* pIDataObject;

	switch( lCommandId )
	{
		case IDM_CREATE_FILE:
			if( SUCCEEDED ( m_pComponent->m_pIFramework->SaveNodeAsNewFile(this) ) )
			{
				hr = S_OK;
			}
			break;

		case IDM_CUT:
			if( CanCut() == S_OK )
			{
				if( SUCCEEDED ( CreateDataObject( &pIDataObject ) ) )
				{
					if( theApp.PutDataInClipboard( pIDataObject, this ) )
					{
						DeleteNode( FALSE );
					}
					
					pIDataObject->Release();
					pIDataObject = NULL;
				}
			}
			break;

		case IDM_COPY:
			if( CanCopy() == S_OK )
			{
				if( SUCCEEDED ( CreateDataObject( &pIDataObject ) ) )
				{
					theApp.PutDataInClipboard( pIDataObject, this );
					
					pIDataObject->Release();
					pIDataObject = NULL;
				}
			}
			break;

		case IDM_PASTE:
		{
			BOOL fWillSetReference;

			if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
			{
				if( CanPasteFromData( pIDataObject, &fWillSetReference ) == S_OK )
				{
					PasteFromData( pIDataObject );
				}
				
				pIDataObject->Release();
				pIDataObject = NULL;
			}
			break;
		}

		case IDM_RENAME:
			if( SUCCEEDED ( m_pComponent->m_pIFramework->EditNodeLabel((IDMUSProdNode *)this) ) )
			{
				hr = S_OK;
			}
			break;

		case IDM_DELETE:
			DeleteNode( TRUE );
			break;

		case IDM_PROPERTIES:
			OnShowProperties();
			hr = S_OK;
			break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::DeleteChildNode

HRESULT CBand::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Band nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::InsertChildNode

HRESULT CBand::InsertChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Band nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::DeleteNode

HRESULT CBand::DeleteNode( BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdNode* pIParentNode;

	// Let our parent delete us
    if( SUCCEEDED ( GetParentNode(&pIParentNode) ) )
	{
		if( pIParentNode )
		{
			// Make sure the band is removed from the notification list
			// A band is added to the notification list when an instrument references a DLS collection
			POSITION position = m_lstInstruments.GetHeadPosition();
			while(position)
			{
				CInstrumentListItem* pInstrumentItem = (CInstrumentListItem*) m_lstInstruments.GetNext(position);
				ASSERT(pInstrumentItem);
				CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
				if(pInstrument)
					pInstrument->SetDLSCollection(NULL);
			}
			HRESULT hr = pIParentNode->DeleteChildNode( this, fPromptUser );
			pIParentNode->Release();
			return hr;
		}
	}

	// No parent so we will delete ourself
	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	// Remove from Project Tree
	if( m_pComponent->m_pIFramework->RemoveNode( this, fPromptUser ) == S_FALSE )
	{
		return E_FAIL;
	}

	// Make sure the band is removed from the notification list
	// A band is added t the notification list when an instrument references a DLS collection
	POSITION position = m_lstInstruments.GetHeadPosition();
	while(position)
	{
		CInstrumentListItem* pInstrumentItem = (CInstrumentListItem*) m_lstInstruments.GetNext(position);
		ASSERT(pInstrumentItem);
		CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
		if(pInstrument)
			pInstrument->SetDLSCollection(NULL);
	}


	// Remove from Component Band list
	m_pComponent->RemoveFromBandFileList( this );

//	m_pComponent->m_pIFramework->SetModifiedFlag( this, TRUE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::OnNodeSelChanged

HRESULT CBand::OnNodeSelChanged( BOOL fSelected )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::CreateDataObject

HRESULT CBand::CreateDataObject( IDataObject** ppIDataObject )
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

	// Save Band into stream
	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( theApp.m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		if( SUCCEEDED ( Save( pIStream, FALSE ) ) )
		{
			// Place CF_BAND into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( m_pComponent->m_cfBand, pIStream ) ) )
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
		if( SUCCEEDED ( theApp.m_pIFramework->SaveClipFormat( m_pComponent->m_cfProducerFile, this, &pIStream ) ) )
		{
			// There is the potential for a Band node to become a file
			// so we must also place CF_DMUSPROD_FILE into CDllJazzDataObject
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
// CBand IDMUSProdNode::CanCut

HRESULT CBand::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return CanDelete();
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::CanCopy

HRESULT CBand::CanCopy( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::CanDelete

HRESULT CBand::CanDelete( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pIDocRootNode != this )
	{
		if( m_pIParentNode )
		{
			return m_pIParentNode->CanDeleteChildNode( this );
		}
		else
		{
			return S_FALSE;
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::CanDeleteChildNode

HRESULT CBand::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( 0 );

	return E_NOTIMPL;	// Band nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::CanPasteFromData

HRESULT CBand::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pfWillSetReference == NULL )
	{
		return E_POINTER;
	}

	*pfWillSetReference = FALSE;

	if( m_pIDocRootNode != this )
	{
		if( m_pIParentNode )
		{
			// Let Style, for example, decide what can be dropped
			return m_pIParentNode->CanChildPasteFromData( pIDataObject, this, pfWillSetReference );
		}
	}

	// Can't drop anything on a Band file in the Project Tree
	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::PasteFromData

HRESULT CBand::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pIDocRootNode != this )
	{
		if( m_pIParentNode )
		{
			// Let Style, for example, handle paste
			return m_pIParentNode->ChildPasteFromData( pIDataObject, this );
		}
	}

	// Can't drop anything on a Band file in the Project Tree
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::CanChildPasteFromData

HRESULT CBand::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
									  BOOL* pfWillSetReference )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;			// Band nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::ChildPasteFromData

HRESULT CBand::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;			// Band nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdNode::GetObject

HRESULT CBand::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdBandEdit implementation

/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdBandEdit::GetDefaultFlag

HRESULT CBand::GetDefaultFlag( BOOL* pfDefaultFlag )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pfDefaultFlag == NULL )
	{
		return E_POINTER;
	}

	*pfDefaultFlag = m_fDefault;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdBandEdit::SetDefaultFlag

HRESULT CBand::SetDefaultFlag( BOOL fDefaultFlag )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_fDefault = fDefaultFlag;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdBandEdit::GetDLSRegionName

HRESULT CBand::GetDLSRegionName( DWORD dwPChannel, BYTE bMIDINote, BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pbstrName == NULL )
	{
		return E_POINTER;
	}

	CInstrumentListItem* pInstrumentItem;
	CDMInstrument* pInstrument;
 
	// Search instrument list for a matching PChannel
	POSITION pos = m_lstInstruments.GetHeadPosition();
	while( pos )
	{
		pInstrumentItem = m_lstInstruments.GetNext( pos );
		pInstrument = pInstrumentItem->GetInstrument();

		if( pInstrument
		&&  pInstrument->dwPChannel == dwPChannel )
		{
			// We found a matching PChannel
			// So check for a DLS collection
			if( pInstrument->m_pDMReference )
			{
				IDLSQueryInstruments* pIDLSQueryInstruments;

				// This PChannel is assigned to a DLS instrument
				// So get a pointer to the DLS Collection's IDLSQueryInstruments interface
				if( SUCCEEDED ( pInstrument->m_pDMReference->QueryInterface( IID_IDLSQueryInstruments, (void **)&pIDLSQueryInstruments ) ) )
				{
					CString strName;
					BSTR bstrName;

					// Now get the Region name
					HRESULT hr = pIDLSQueryInstruments->GetRegionWaveName( (BYTE)MSB( pInstrument->dwPatch ),
																		   (BYTE)LSB( pInstrument->dwPatch ),	
																		   (BYTE)( pInstrument->dwPatch & 0x7F ),
																			pInstrument->IsDrums(),	
																			bMIDINote,
																			&bstrName );
					pIDLSQueryInstruments->Release();
					pIDLSQueryInstruments = NULL;

					if( hr == S_OK )
					{
						strName = bstrName;
						::SysFreeString( bstrName );

						*pbstrName = strName.AllocSysString();
						return S_OK;

					}
				}
			}

			// There aren't DLS region names for this PChannel
			break;
		}
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdBandEdit8 implementation

/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdBandEdit8::GetPatchForPChannel

HRESULT CBand::GetPatchForPChannel( DWORD dwPChannel, DWORD *pdwPatch )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pdwPatch == NULL )
	{
		return E_POINTER;
	}

	// Search instrument list for a matching PChannel
	POSITION pos = m_lstInstruments.GetHeadPosition();
	while( pos )
	{
		CInstrumentListItem* pInstrumentItem = m_lstInstruments.GetNext( pos );
		CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();

		if( pInstrument
		&&  pInstrument->dwPChannel == dwPChannel )
		{
			if( pInstrument->dwFlags & DMUS_IO_INST_PATCH )
			{
				// We found a matching PChannel - copy the patch number
				*pdwPatch = pInstrument->dwPatch;
				return S_OK;
			}
			else
			{
				// Instrument doesn't have a patch set
				return DMUS_E_TYPE_DISABLED;
			}
		}
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdBandEdit8a implementation

/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdBandEdit8a::GetInstNameForPChannel

HRESULT CBand::GetInstNameForPChannel( DWORD dwPChannel, BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pbstrName == NULL )
	{
		return E_POINTER;
	}

	// Search instrument list for a matching PChannel
	POSITION pos = m_lstInstruments.GetHeadPosition();
	while( pos )
	{
		CInstrumentListItem* pInstrumentItem = m_lstInstruments.GetNext( pos );
		CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();

		if( pInstrument
		&&  pInstrument->dwPChannel == dwPChannel )
		{
			if( pInstrument->dwFlags & DMUS_IO_INST_PATCH )
			{
				// We found a matching PChannel - determine the instrument name
				CString strName = CBandDlg::GetPatchName( m_pComponent, pInstrument);
				if( !strName.IsEmpty() )
				{
					*pbstrName = strName.AllocSysString();
					return S_OK;
				}
				else
				{
					// Instrument doesn't have a name set
					return E_FAIL;
				}
			}
			else
			{
				// Instrument doesn't have a patch set
				return DMUS_E_TYPE_DISABLED;
			}
		}
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdBandEdit8a::DisplayInstrumentButton

HRESULT CBand::DisplayInstrumentButton( DWORD dwPChannel, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Search instrument list for a matching PChannel
	POSITION pos = m_lstInstruments.GetHeadPosition();
	while( pos )
	{
		CInstrumentListItem* pInstrumentItem = m_lstInstruments.GetNext( pos );
		CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();

		if( pInstrument
		&&  pInstrument->dwPChannel == dwPChannel )
		{
			if( pInstrument->dwFlags & DMUS_IO_INST_PATCH )
			{
				// We found a matching PChannel - display the popup menu
				HMENU hmenu = CPChannelPropPage::m_hmenuDrums;

				if(dwPChannel != 9 && dwPChannel%16 != 9)
					hmenu = CPChannelPropPage::m_hmenuPatch;

				m_pDMInstrumentForContextMenu = pInstrument;
				m_dwPChannelForContextMenu = dwPChannel;

				TrackPopupMenu( GetSubMenu( hmenu, 0 ), TPM_LEFTALIGN | TPM_VCENTERALIGN | TPM_LEFTBUTTON,
											lXPos, lYPos, 0, m_ContextMenuHandler.GetSafeHwnd(), NULL );
				return S_OK;
			}
			else
			{
				// Instrument doesn't have a patch set
				return DMUS_E_TYPE_DISABLED;
			}
		}
	}

	return E_INVALIDARG;
}

HRESULT CBand::OnContextCommand( WPARAM wParam, LPARAM lParam )
{
	// not a menu message
	if( lParam )  
	{
		return E_NOTIMPL;
	}

	if( wParam == ID_INSERT_NEWINSTRUMENT )
	{
		return InsertPChannel( m_dwPChannelForContextMenu, FALSE );
	}
	else if( wParam == ID_OPEN_BANDEDITOR )
	{
		return m_pComponent->m_pIFramework->OpenEditor( this );
	}

	//
	// This is very similar to CPChannelPropPage::OnCommand - please try to keep them in sync!
	//

	// check for program changes
	BInstr  *pbi;

	pbi = &abiGS[0];
	int nCount = 0;
	while( pbi->nStringId != IDS_PGMEND )
	{
		if( pbi->nStringId == wParam )
		{
			// Save the state for undo
			// Not necessary - this is only called if we're embedded in a segment,
			// so the segment handles the Undo queue
			//pBandDlg->SaveStateForUndo("Program Change");

			CString sName = *pbi->pstrName;

			const DWORD dwOrigPatch = m_pDMInstrumentForContextMenu->dwPatch;
			const IDMUSProdReferenceNode *pOrigRefNode = m_pDMInstrumentForContextMenu->m_pDMReference;

			CInstrumentListItem::m_CommonPropertyObject.SetPatch(pbi->bPatch);
			CInstrumentListItem::m_CommonPropertyObject.SetInstrument(sName);
			CInstrumentListItem::m_CommonPropertyObject.m_dwChanged = INSTRUMENT_CHANGED;
			CInstrumentListItem::m_CommonPropertyObject.m_dwInstrumentChangeID = 0;
			CInstrumentListItem::m_CommonPropertyObject.m_nABIIndex = nCount;

			CInstrumentListItem::m_CommonPropertyObject.ApplyInstrumentChange( m_pDMInstrumentForContextMenu );

			// Check if anything actually changed
			if( dwOrigPatch == m_pDMInstrumentForContextMenu->dwPatch
			&&	pOrigRefNode == m_pDMInstrumentForContextMenu->m_pDMReference )
			{
				return S_FALSE;
			}

			// We need to update if somebody else does a Send
			CDMInstrument::m_bSendBandUpdate = TRUE;
			if(m_pDMInstrumentForContextMenu->SendBandUpdate(this))
			{
				CDMInstrument::m_bSendBandUpdate = FALSE;
				CInstrumentListItem::m_CommonPropertyObject.SyncChanges(m_pDMInstrumentForContextMenu, this);
			}
			CDMInstrument::m_bSendBandUpdate = TRUE;
			SetModifiedFlag( TRUE );

			// Refresh the display
			if( m_pBandCtrl
			&&	m_pBandCtrl->m_pBandDlg )
			{
				m_pBandCtrl->m_pBandDlg->UpdatePChannelChanges(true, false);
			}

			return S_OK;
		}
		pbi++;
		nCount++;
	}

	// check for drum program changes
	pbi = &abiDrums[0];
	nCount = 0;
	while( pbi->nStringId != IDS_PGMEND )
	{
		if( pbi->nStringId == wParam )
		{
			// Save the state for undo
			// Not necessary - this is only called if we're embedded in a segment,
			// so the segment handles the Undo queue
			//pBandDlg->SaveStateForUndo("Program Change");

			CString sName = *pbi->pstrName;

			const DWORD dwOrigPatch = m_pDMInstrumentForContextMenu->dwPatch;
			const IDMUSProdReferenceNode *pOrigRefNode = m_pDMInstrumentForContextMenu->m_pDMReference;

			CInstrumentListItem::m_CommonPropertyObject.SetPatch(pbi->bPatch);
			CInstrumentListItem::m_CommonPropertyObject.SetInstrument(sName);
			CInstrumentListItem::m_CommonPropertyObject.m_dwChanged = INSTRUMENT_CHANGED;
			CInstrumentListItem::m_CommonPropertyObject.m_dwInstrumentChangeID = DRUMS_REQUEST;
			CInstrumentListItem::m_CommonPropertyObject.m_nABIIndex = nCount;

			CInstrumentListItem::m_CommonPropertyObject.ApplyInstrumentChange( m_pDMInstrumentForContextMenu );

			// Check if anything actually changed
			if( dwOrigPatch == m_pDMInstrumentForContextMenu->dwPatch
			&&	pOrigRefNode == m_pDMInstrumentForContextMenu->m_pDMReference )
			{
				return S_FALSE;
			}

			// We need to update if somebody else does a Send
			CDMInstrument::m_bSendBandUpdate = TRUE;
			if(m_pDMInstrumentForContextMenu->SendBandUpdate(this))
			{
				CDMInstrument::m_bSendBandUpdate = FALSE;
				CInstrumentListItem::m_CommonPropertyObject.SyncChanges(m_pDMInstrumentForContextMenu, this);
			}
			CDMInstrument::m_bSendBandUpdate = TRUE;
			SetModifiedFlag( TRUE );

			// Refresh the display
			if( m_pBandCtrl
			&&	m_pBandCtrl->m_pBandDlg )
			{
				m_pBandCtrl->m_pBandDlg->UpdatePChannelChanges(true, false);
			}

			return S_OK;
		}
		pbi++;
		nCount++;
	}

	// Check if it's other DLS request
	if( wParam == IDS_DLS )
	{
		// Save the state for undo
		// Not necessary - this is only called if we're embedded in a segment,
		// so the segment handles the Undo queue
		//pBandDlg->SaveStateForUndo("Program Change");

		const DWORD dwOrigPatch = m_pDMInstrumentForContextMenu->dwPatch;
		const IDMUSProdReferenceNode *pOrigRefNode = m_pDMInstrumentForContextMenu->m_pDMReference;

		CInstrumentListItem::m_CommonPropertyObject.m_dwInstrumentChangeID = DLS_REQUEST;
		CInstrumentListItem::m_CommonPropertyObject.m_dwChanged = INSTRUMENT_CHANGED;

		CInstrumentListItem::m_CommonPropertyObject.ApplyInstrumentChange( m_pDMInstrumentForContextMenu, &m_strDefaultCollection );

		// Check if anything actually changed
		if( dwOrigPatch == m_pDMInstrumentForContextMenu->dwPatch
		&&	pOrigRefNode == m_pDMInstrumentForContextMenu->m_pDMReference )
		{
			return S_FALSE;
		}

		// We need to update if somebody else does a Send
		CDMInstrument::m_bSendBandUpdate = TRUE;
		if(m_pDMInstrumentForContextMenu->SendBandUpdate(this))
		{
			CDMInstrument::m_bSendBandUpdate = FALSE;
			CInstrumentListItem::m_CommonPropertyObject.SyncChanges(m_pDMInstrumentForContextMenu, this);
		}
		CDMInstrument::m_bSendBandUpdate = TRUE;
		SetModifiedFlag( TRUE );

		// Refresh the display
		if( m_pBandCtrl
		&&	m_pBandCtrl->m_pBandDlg )
		{
			m_pBandCtrl->m_pBandDlg->UpdatePChannelChanges(true, false);
		}

		return S_OK;
	}

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdBandEdit8a::InsertPChannel

HRESULT CBand::InsertPChannel( DWORD dwPChannel, BOOL fClearBandFirst )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( dwPChannel > 999 )
	{
		return E_INVALIDARG;
	}

	if( fClearBandFirst )
	{
		DeleteInstrumentList();
	}

	// Search instrument list for a matching PChannel
	CInstrumentListItem* pMatchingInstrumentItem = NULL;
	POSITION pos = m_lstInstruments.GetHeadPosition();
	while( pos )
	{
		CInstrumentListItem* pInstrumentItem = m_lstInstruments.GetNext( pos );
		CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();

		if( pInstrument
		&&  pInstrument->dwPChannel == dwPChannel )
		{
			if( pInstrument->dwFlags & DMUS_IO_INST_PATCH )
			{
				// We found a matching PChannel - fail
				return E_INVALIDARG;
			}
			else
			{
				pMatchingInstrumentItem = pInstrumentItem;
				break;
			}
		}
	}

	if( pMatchingInstrumentItem )
	{
		// If there already is an instrument for this channel
		CDMInstrument* pInstrument = pMatchingInstrumentItem->GetInstrument();
		pInstrument->ResetInstruemnt();
		pInstrument->dwPatch = MAKE_PATCH(pInstrument->IsDrums(), 0, 0, 0);
		int nPriorityIndex = pInstrument->dwPChannel%16;
		pInstrument->dwChannelPriority = dwaPriorities[nPriorityIndex];
		pInstrument->dwFlags = DMUS_IO_INST_GM | DMUS_IO_INST_PITCHBENDRANGE | DMUS_IO_INST_TRANSPOSE | DMUS_IO_INST_VOLUME | DMUS_IO_INST_PAN | DMUS_IO_INST_BANKSELECT | DMUS_IO_INST_PATCH;
		pInstrument->SendBandUpdate(this);
	}
	else
	{
		// No matching instrument - need to create a new one
		POSITION position = m_lstInstruments.FindIndex(dwPChannel - 1);
		CDMInstrument* pInstrument = new CDMInstrument();
		pInstrument->SetComponent(m_pComponent);
		pInstrument->SetBand(this);
		pInstrument->dwPChannel = dwPChannel;
		pInstrument->dwPatch = MAKE_PATCH(pInstrument->IsDrums(), 0, 0, 0);
		int nPriorityIndex = pInstrument->dwPChannel%16;
		pInstrument->dwChannelPriority = dwaPriorities[nPriorityIndex];
		pInstrument->dwFlags = DMUS_IO_INST_GM | DMUS_IO_INST_PITCHBENDRANGE | DMUS_IO_INST_TRANSPOSE | DMUS_IO_INST_VOLUME | DMUS_IO_INST_PAN | DMUS_IO_INST_BANKSELECT | DMUS_IO_INST_PATCH;
		pMatchingInstrumentItem = new CInstrumentListItem(pInstrument);
		m_lstInstruments.InsertAfter(position, pMatchingInstrumentItem);
		pInstrument->SendBandUpdate(this);
	}

	CDMInstrument* pInstrument = pMatchingInstrumentItem->GetInstrument();
	ASSERT(pInstrument);
	pInstrument->Send(this, DM_PATCH_CHANGE);
	SetModifiedFlag( TRUE );

	// Refresh the display
	if( m_pBandCtrl
	&&	m_pBandCtrl->m_pBandDlg )
	{
		m_pBandCtrl->m_pBandDlg->UpdatePChannelChanges(true, false);
	}

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdBandEdit8a::SetAudiopath

HRESULT CBand::SetAudiopath( IUnknown* punkAudiopath )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	RELEASE( m_pAudiopath );

	if( punkAudiopath )
	{
		return punkAudiopath->QueryInterface( IID_IDirectMusicAudioPath, (void **)&m_pAudiopath );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdBandMgrEdit implementation

/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdBandMgrEdit::DisplayEditBandButton

HRESULT CBand::DisplayEditBandButton( DWORD dwPChannel, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Enable the "New Instrument" item if this is true
	bool fEnableNewInstrument = true;

	// Search instrument list for a matching PChannel
	POSITION pos = m_lstInstruments.GetHeadPosition();
	while( pos )
	{
		CInstrumentListItem* pInstrumentItem = m_lstInstruments.GetNext( pos );
		CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();

		if( pInstrument
		&&  pInstrument->dwPChannel == dwPChannel )
		{
			if( pInstrument->dwFlags & DMUS_IO_INST_PATCH )
			{
				// We found a matching PChannel - set fEnableNewInstrument to false
				fEnableNewInstrument = false;
				break;
			}
			else
			{
				// We found a matching PChannel, but it doesn't have a patch
				// Keep fEnableNewInstrument at true
				break;
			}
		}
	}

	// Load the menu
	HMENU hMenu = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDR_BANDMENU) );
	HMENU hMenuPopup = ::GetSubMenu(hMenu, 0);

	// Enable the items appropriately
	::EnableMenuItem( hMenuPopup, ID_OPEN_BANDEDITOR, MF_ENABLED );
	::EnableMenuItem( hMenuPopup, ID_INSERT_NEWINSTRUMENT, fEnableNewInstrument ? MF_ENABLED : MF_GRAYED );

	// Save the PChannel # and instrument
	m_dwPChannelForContextMenu = dwPChannel;
	m_pDMInstrumentForContextMenu = NULL; // Not used

	// Display the menu
	TrackPopupMenu( hMenuPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, lXPos, lYPos, 0, m_ContextMenuHandler.GetSafeHwnd(), NULL );

	// Cleanup
	::DestroyMenu( hMenu );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CBand IPersist::GetClassID

HRESULT CBand::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( pClsId != NULL );

    memset( pClsId, 0, sizeof( CLSID ) );

    return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
// CBand IPersistStream::IsDirty

HRESULT CBand::IsDirty()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return ( m_fDirty ) ? S_OK : S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IPersistStream::Load

HRESULT CBand::Load( IStream* pIStream )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pBandCtrl	&&  m_pBandCtrl->m_pBandDlg )
	{
		m_pBandCtrl->m_pBandDlg->m_MixGrid.RemoveAllButtons();
	}

	IDMUSProdRIFFStream* pIRiffStream;
	MMCKINFO ckMain;

	ASSERT( pIStream != NULL );
	ASSERT( m_pComponent != NULL );


	HRESULT hr = E_FAIL;

	if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
        if( pIRiffStream->Descend( &ckMain, NULL, 0 ) == 0 )
        {
			if (ckMain.ckid == FOURCC_RIFF)
			{
				if (ckMain.fccType == FOURCC_BAND_FORM)
				{
					hr = Load_IMA_Band( pIRiffStream, &ckMain );
				}
				else if (ckMain.fccType == DMUS_FOURCC_BAND_FORM)
				{
					hr = LoadDMBand( pIRiffStream, &ckMain );
				}
			}
        }

		pIRiffStream->Release();
    }

	// Update the mixer grid...
	// Load could be called through the Undo call thread...
	if( m_pBandCtrl	&&  m_pBandCtrl->m_pBandDlg )
	{
		m_pBandCtrl->m_pBandDlg->RemoveAndAddAllButtonsToGrid();
	}

    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CBand IPersistStream::Save

HRESULT CBand::Save( IStream* pIStream, BOOL fClearDirty )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    IDMUSProdRIFFStream* pIRiffStream;
    HRESULT hr = E_FAIL;
    MMCKINFO ckMain;

	// Save the Band
	if( FAILED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return E_FAIL;
	}
		
	ckMain.fccType = DMUS_FOURCC_BAND_FORM;
	if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATERIFF ) == 0
		&&  SUCCEEDED( SaveBandChunk( pIRiffStream ) )
		&&  pIRiffStream->Ascend( &ckMain, 0 ) == 0 )
	{
		if( fClearDirty )
		{
			SetModifiedFlag( FALSE );
		}
		
		hr = S_OK;
	}
	pIRiffStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IPersistStream::GetSizeMax

HRESULT CBand::GetSizeMax( ULARGE_INTEGER FAR* /*pcbSize*/ )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CBand INotifySink Implementation

/////////////////////////////////////////////////////////////////////////////
// CBand::OnUpdate

HRESULT CBand::OnUpdate( IDMUSProdNode* pIDocRootNode, GUID guidUpdateType, VOID* pData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ::IsEqualGUID( guidUpdateType, GUID_BAND_UpdatePerformanceMsg ))
	{
		if( pIDocRootNode == m_pIDocRootNode )
		{
			SendBand();
			return S_OK;
		}
		return E_INVALIDARG;
	}

	if(::IsEqualGUID( guidUpdateType, GUID_DMCollectionResync ))
	{
		IDMUSProdNode* pIDocRootNodeDLS = NULL;
		CDMInstrument* pInstrument = NULL;
		CInstrumentListItem *pInstrumentItem = NULL;

		// A DLS Collection used by this Band is resyncing it's IDirectMusicCollection object
		POSITION position = m_lstInstruments.GetHeadPosition();
		while( position )
		{
			pInstrumentItem = m_lstInstruments.GetNext( position );
			pInstrument = pInstrumentItem->GetInstrument();

			if( pInstrument != NULL && pInstrument->m_pDMReference )
			{
				if( SUCCEEDED (	pInstrument->m_pDMReference->GetReferencedFile( &pIDocRootNodeDLS ) ) )
				{
					if( pIDocRootNodeDLS == pIDocRootNode )
					{
						SendBand();
						pIDocRootNodeDLS->Release();
						break;
					}

					pIDocRootNodeDLS->Release();
				}
			}
		}

		return S_OK;
	}

	else if( ::IsEqualGUID(guidUpdateType, FRAMEWORK_FileNameChange )
		 ||  ::IsEqualGUID(guidUpdateType, DOCROOT_GuidChange ) 
		 ||  ::IsEqualGUID(guidUpdateType, COLLECTION_NameChange))
	{
		IDMUSProdNode* pIDocRootNodeDLS;

		CDMInstrument* pInstrument;
		CInstrumentListItem *pInstrumentItem;

		// A DLS Collection used by this Band was renamed or had its GUID changed
		POSITION position = m_lstInstruments.GetHeadPosition();
		while( position )
		{
			pInstrumentItem = m_lstInstruments.GetNext( position );
			pInstrument = pInstrumentItem->GetInstrument();

			if( pInstrument != NULL && pInstrument->m_pDMReference )
			{
				if( SUCCEEDED (	pInstrument->m_pDMReference->GetReferencedFile( &pIDocRootNodeDLS ) ) )
				{
					if( pIDocRootNodeDLS == pIDocRootNode )
					{
						SetModifiedFlag( TRUE );
						// No need to change text on patch button, it didn't change
						// but change the collection name on the static
						BSTR bstrDLSName;
						pIDocRootNodeDLS->GetNodeName(&bstrDLSName);
						pInstrument->m_csCollection = bstrDLSName;
					}

					pIDocRootNodeDLS->Release();
				}
			}
		}

		return S_OK;
	}

	else if( ::IsEqualGUID( guidUpdateType, FRAMEWORK_FileDeleted )
		 ||  ::IsEqualGUID( guidUpdateType, FRAMEWORK_FileClosed ) )
	{
		IDMUSProdNode* pIDocRootNodeDLS;

		CDMInstrument* pInstrument;
		CInstrumentListItem *pInstrumentItem;

		// A DLS Collection used by this Band was deleted or closed
		POSITION position = m_lstInstruments.GetHeadPosition();
		while( position )
		{
			pInstrumentItem = m_lstInstruments.GetNext( position );
			pInstrument = pInstrumentItem->GetInstrument();

			if( pInstrument != NULL && pInstrument->m_pDMReference )
			{
				if( SUCCEEDED (	pInstrument->m_pDMReference->GetReferencedFile( &pIDocRootNodeDLS ) ) )
				{
					if( pIDocRootNodeDLS == pIDocRootNode )
					{
						pInstrument->SetDLSCollection( NULL );

						if( ::IsEqualGUID( guidUpdateType, FRAMEWORK_FileDeleted ) )
						{
							SetModifiedFlag( TRUE );
						}

						// Change text on patch button
						if( m_pBandCtrl	&& m_pBandCtrl->m_pBandDlg )
						{
							long lOrigTrackEdit = m_pBandCtrl->m_pBandDlg->m_nTrackEdit;

							m_pBandCtrl->m_pBandDlg->m_nTrackEdit = pInstrument->dwPChannel + 1;
							m_pBandCtrl->m_pBandDlg->UpdatePChannelList();
							m_pBandCtrl->m_pBandDlg->RefreshPChannelPropertyPage();
							
							m_pBandCtrl->m_pBandDlg->m_nTrackEdit = lOrigTrackEdit; 
						}
					}

					pIDocRootNodeDLS->Release();
				}
			}
		}

		// Refresh Band Editor
		if( m_pBandCtrl
		&&  m_pBandCtrl->m_pBandDlg )
		{
			m_pBandCtrl->m_pBandDlg->InvalidateRect( NULL );
			m_pBandCtrl->m_pBandDlg->UpdateWindow();
		}

		return S_OK;
	}

	else if( ::IsEqualGUID(guidUpdateType, FRAMEWORK_FileReplaced ) )
	{
		IDMUSProdNode* pIDocRootNodeDLS;

		CDMInstrument* pInstrument;
		CInstrumentListItem *pInstrumentItem;

		// A DLS Collection used by this Band was replaced in the Project Tree
		POSITION position = m_lstInstruments.GetHeadPosition();
		while( position )
		{
			pInstrumentItem = m_lstInstruments.GetNext( position );
			pInstrument = pInstrumentItem->GetInstrument();

			if( pInstrument != NULL && pInstrument->m_pDMReference )
			{
				if( SUCCEEDED (	pInstrument->m_pDMReference->GetReferencedFile( &pIDocRootNodeDLS ) ) )
				{
					if( pIDocRootNodeDLS == pIDocRootNode )
					{
						IDMUSProdReferenceNode* pCollectionRef = m_pComponent->CreateCollectionRefNode( (IDMUSProdNode *)pData );
						if( pCollectionRef )
						{
							pInstrument->SetDLSCollection( pCollectionRef );
							SetModifiedFlag( TRUE );

							// Change text on patch button
							if( m_pBandCtrl
							&&  m_pBandCtrl->m_pBandDlg )
							{
								long lOrigTrackEdit = m_pBandCtrl->m_pBandDlg->m_nTrackEdit;

								m_pBandCtrl->m_pBandDlg->m_nTrackEdit = pInstrument->dwPChannel + 1;
								m_pBandCtrl->m_pBandDlg->UpdatePChannelList();
								
								m_pBandCtrl->m_pBandDlg->m_nTrackEdit = lOrigTrackEdit; 
							}

							pCollectionRef->Release();
						}
					}

					pIDocRootNodeDLS->Release();
				}
			}
		}

		// Refresh Band Editor
		if( m_pBandCtrl	&&  m_pBandCtrl->m_pBandDlg )
		{
			m_pBandCtrl->m_pBandDlg->InvalidateRect( NULL );
			m_pBandCtrl->m_pBandDlg->UpdateWindow();
		}

		return S_OK;
	}
	else if(::IsEqualGUID(guidUpdateType, INSTRUMENT_NameChange) ||
			::IsEqualGUID(guidUpdateType, INSTRUMENT_Deleted))
	{
		// Just refresh the dialog
		if( m_pBandCtrl	&&  m_pBandCtrl->m_pBandDlg )
		{
			m_pBandCtrl->m_pBandDlg->m_PChannelList.Invalidate();
			if(::IsEqualGUID(guidUpdateType, INSTRUMENT_Deleted))
				m_pBandCtrl->m_pBandDlg->RefreshDisplay();
		}
		
		return S_OK;
	}

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CBand additional functions

/////////////////////////////////////////////////////////////////////////////
// CBand::SendBand

void CBand::SendBand( )
{
	IDirectMusicBand* pIDMBand;

	CWaitCursor wait;

	// Persist the Band into a DirectMusicBand object
	if( SUCCEEDED ( ::CoCreateInstance( CLSID_DirectMusicBand, NULL, CLSCTX_INPROC, IID_IDirectMusicBand, (void**)&pIDMBand ) ) )
	{
		IStream* pIMemStream;
		IPersistStream* pIPersistStreamEngine;

		if( SUCCEEDED ( m_pComponent->m_pIFramework->AllocMemoryStream( FT_RUNTIME, GUID_DirectMusicObject, &pIMemStream ) ) )
		{
			if( SUCCEEDED ( Save( pIMemStream, FALSE ) ) )
			{
				if( SUCCEEDED ( pIDMBand->QueryInterface( IID_IPersistStream, (void **)&pIPersistStreamEngine ) ) )
				{
					StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );
					pIPersistStreamEngine->Load( pIMemStream );

					// Send the Band
					IDirectMusicSegment* pIDMSegmentBand;

					if( SUCCEEDED ( pIDMBand->CreateSegment( &pIDMSegmentBand ) ) )
					{
						HRESULT hr = m_pComponent->m_pIDMPerformance->PlaySegment( pIDMSegmentBand,
								 DMUS_SEGF_SECONDARY, 0, NULL );
						pIDMSegmentBand->Release();
					}

					pIPersistStreamEngine->Release();
				}
			}

			pIMemStream->Release();
		}

		pIDMBand->Release();
	}
}

HRESULT CBand::NotifyEveryone( GUID guidNotification)
{
	ASSERT(m_pIDocRootNode);
	if(m_pIDocRootNode == NULL)
	{
		return E_UNEXPECTED;
	}

	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		return E_UNEXPECTED;
	}

	if( m_pIDocRootNode != this )
	{
		IDMUSProdNotifySink* pINotifySink = NULL;
		if( SUCCEEDED ( m_pIDocRootNode->QueryInterface(IID_IDMUSProdNotifySink, (void**)&pINotifySink) ) )
		{
			// Ask DocRoot if change should be sent
			if( pINotifySink->OnUpdate( this, guidNotification, NULL ) == S_OK )
			{
				SendBand();
			}

			pINotifySink->Release();
		}
	}

	IDMUSProdFramework* pIFramework = m_pComponent->m_pIFramework;
	ASSERT(pIFramework);
	if(pIFramework == NULL)
	{
		return E_UNEXPECTED;
	}

	pIFramework->NotifyNodes(this, guidNotification, NULL);
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand::Activate

void CBand::Activate( void )
{
	// Notify DocRoot that this Band Editor is the active window
	if( m_pIDocRootNode != (IDMUSProdNode *)this )
	{
		IDMUSProdNotifySink* pINotifySink;

		if( SUCCEEDED ( m_pIDocRootNode->QueryInterface(IID_IDMUSProdNotifySink, (void**)&pINotifySink) ) )
		{
			// Ask DocRoot if change should be sent
			if( pINotifySink->OnUpdate( this, GUID_BAND_ActivateNotifyMsg, NULL ) == S_OK )
			{
				SendBand();
			}

			pINotifySink->Release();
		}
	}
	m_pBandCtrl->m_pBandDlg->m_PChannelList.SetFocus();
}


/////////////////////////////////////////////////////////////////////////////
// CBand::EnablePatch

void CBand::EnablePatch( int nIndex )
{
	CDMInstrument* pInstrument = GetInstrumentFromList(nIndex);
	
	ASSERT(pInstrument);
	if(pInstrument == NULL)
		return;

	pInstrument->dwFlags |= 
				(DMUS_IO_INST_TRANSPOSE | DMUS_IO_INST_PAN | DMUS_IO_INST_VOLUME | DMUS_IO_INST_PATCH);
}


/////////////////////////////////////////////////////////////////////////////
// CBand::RemoveFromNotifyList

void CBand::RemoveFromNotifyList( IDMUSProdReferenceNode* pDMReferenceNode )
{
	CLinkedDLSCollection* pTheLinkedDLSCollection = NULL;
	IDMUSProdNode* pIDocRootNode;

	if( SUCCEEDED (	pDMReferenceNode->GetReferencedFile( &pIDocRootNode ) ) )
	{
		CLinkedDLSCollection* pLinkedDLSCollection;

		POSITION pos = m_lstDLSCollections.GetHeadPosition();
		while( pos )
		{
			pLinkedDLSCollection = m_lstDLSCollections.GetNext( pos );

			if( pLinkedDLSCollection->m_pIDocRootNode == pIDocRootNode )
			{
				pTheLinkedDLSCollection = pLinkedDLSCollection;
				break;
			}
		}

		if( pTheLinkedDLSCollection )
		{
			ASSERT( pTheLinkedDLSCollection->m_dwUseCount > 0 );

			pTheLinkedDLSCollection->m_dwUseCount--;

			if( pTheLinkedDLSCollection->m_dwUseCount == 0 )
			{
				// Remove from DLS Collection's notify list
				theApp.m_pIFramework->RemoveFromNotifyList( pIDocRootNode, this );

				// Remove from Band's DLS Collection list
				pos = m_lstDLSCollections.Find( pTheLinkedDLSCollection );
				if( pos )
				{
					m_lstDLSCollections.RemoveAt( pos );
					delete pTheLinkedDLSCollection;
				}
			}
		}

		pIDocRootNode->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CBand::AddToNotifyList

void CBand::AddToNotifyList( IDMUSProdReferenceNode* pDMReferenceNode )
{
	CLinkedDLSCollection* pTheLinkedDLSCollection = NULL;
	IDMUSProdNode* pIDocRootNode;

	if( SUCCEEDED (	pDMReferenceNode->GetReferencedFile( &pIDocRootNode ) ) )
	{
		CLinkedDLSCollection* pLinkedDLSCollection;

		POSITION pos = m_lstDLSCollections.GetHeadPosition();
		while( pos )
		{
			pLinkedDLSCollection = m_lstDLSCollections.GetNext( pos );

			if( pLinkedDLSCollection->m_pIDocRootNode == pIDocRootNode )
			{
				pTheLinkedDLSCollection = pLinkedDLSCollection;
				break;
			}
		}

		if( pTheLinkedDLSCollection )
		{
			pTheLinkedDLSCollection->m_dwUseCount++;
		}
		else
		{
			pTheLinkedDLSCollection = new CLinkedDLSCollection( pIDocRootNode );
			if( pTheLinkedDLSCollection )
			{
				// Add to DLS Collection file's notify list
				if( SUCCEEDED ( theApp.m_pIFramework->AddToNotifyList( pIDocRootNode, this ) ) )
				{
					m_lstDLSCollections.AddTail( pTheLinkedDLSCollection );
				}
				else
				{
					delete pTheLinkedDLSCollection;
				}
			}
		}

		pIDocRootNode->Release();
	}
}


CDMInstrument* CBand::GetInstrumentFromList(long lPChannel)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	if(m_lstInstruments.IsEmpty())
		return NULL;

	CInstrumentListItem* pInstrumentItem = GetInstrumentItemFromList(lPChannel);
	if(pInstrumentItem)
	{
		CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
		ASSERT(pInstrument);
		return pInstrument;
	}
	return NULL;
}

CInstrumentListItem* CBand::GetInstrumentItemFromList(long lPChannel)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	if(lPChannel < 0 || m_lstInstruments.IsEmpty())
		return NULL;
	
	POSITION position = m_lstInstruments.GetHeadPosition();
	while(position)
	{
		CInstrumentListItem* pInstrumentListItem = (CInstrumentListItem*)m_lstInstruments.GetNext(position);
		if(pInstrumentListItem == NULL)
			break;

		CDMInstrument* pInstrument = pInstrumentListItem->GetInstrument();
		if(pInstrument->dwPChannel == (DWORD)lPChannel)
			return pInstrumentListItem;
	}
	return NULL;
}

int CBand::GetInstrumentCount()
{
	return m_lstInstruments.GetCount();
}


//////////////////////////////////////////////////////
// CBand::GetInstrumentByIndex(int nIndex)
//
// This just gets instruments based on the passed index and NOT on the PChannel
// This method assumes that the passed index would exist and returns NULL if
// an instrument at that index was not found.
CDMInstrument* CBand::GetInstrumentByIndex(int nIndex)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	if(m_lstInstruments.IsEmpty())
		return NULL;
	
	POSITION position = m_lstInstruments.FindIndex(nIndex);
	CInstrumentListItem* pInstrumentListItem = m_lstInstruments.GetAt(position);
	CDMInstrument* pInstrument = pInstrumentListItem->GetInstrument();
	return pInstrument;
}

CInstrumentListItem* CBand::GetInstrumentItemByIndex(int nCount)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CInstrumentListItem* pInstrumentListItem = NULL;
	
	POSITION position = m_lstInstruments.FindIndex(nCount);
	pInstrumentListItem = m_lstInstruments.GetAt(position);
	ASSERT(pInstrumentListItem);
	
	return pInstrumentListItem;
	
}
	
int CBand::GetNumberOfSelectedInstruments()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	int nCount = 0;
	CInstrumentListItem* pInstrumentItem = NULL;
	POSITION position = m_lstInstruments.GetHeadPosition();

	while(position)
	{
		pInstrumentItem = m_lstInstruments.GetNext(position);
		if(pInstrumentItem->IsSelected())
		{
			nCount++;
		}
	}
	return nCount;
}


int CBand::GetSelectedInstruments(CInstrumentListItem** ppInstrumentArray)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(ppInstrumentArray);

	int nCount = 0;
	CInstrumentListItem* pInstrumentItem;
	POSITION position = m_lstInstruments.GetHeadPosition();

	while(position)
	{
		pInstrumentItem = m_lstInstruments.GetNext(position);
		if(pInstrumentItem->IsSelected())
		{
			ppInstrumentArray[nCount] = pInstrumentItem;
			nCount++;
		}
	}
	return nCount;
}

void CBand::SelectAllInstruments()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	POSITION position = m_lstInstruments.GetHeadPosition();
	while(position)
	{
		CInstrumentListItem* pInstrumentItem = (CInstrumentListItem*)m_lstInstruments.GetNext(position);
		ASSERT(pInstrumentItem);
		pInstrumentItem->SetSelected(true);
	}
}

void CBand::UnselectAllInstruments()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CInstrumentListItem* pInstrumentItem;
	POSITION position = m_lstInstruments.GetHeadPosition();

	while(position)
	{
		pInstrumentItem = m_lstInstruments.GetNext(position);
		if(pInstrumentItem->IsSelected())
		{
			pInstrumentItem->SetSelected(false);
		}
	}

}


DWORD CBand::GetNextAvailablePChannel(DWORD dwStartChannel)
{
	DWORD dwHighestPChannelInList = 0;

	POSITION position = m_lstInstruments.GetHeadPosition();
	DWORD dwLastPChannel = dwStartChannel;
	
	while(position)
	{
		CInstrumentListItem* pInstrumentItem = (CInstrumentListItem*)m_lstInstruments.GetNext(position);
		ASSERT(pInstrumentItem);
		CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
		ASSERT(pInstrument);

		DWORD dwPChannelInList = pInstrument->dwPChannel + 1;
		
		// If a vacant position return the last position + 1
		if(dwLastPChannel < dwPChannelInList - 1)		
		{
			return dwLastPChannel;						
		}

		if(dwLastPChannel < dwPChannelInList)
		{
			dwLastPChannel = dwPChannelInList;
		}

		if(dwStartChannel <= dwPChannelInList)
		{
			dwLastPChannel = dwPChannelInList;
		}

		if(dwHighestPChannelInList < dwLastPChannel)
		{
			dwHighestPChannelInList = dwLastPChannel;
		}
	}
	
	if(dwLastPChannel == dwStartChannel)
	{
		dwHighestPChannelInList = dwLastPChannel;
	}
	
	return (dwHighestPChannelInList);	

}


DWORD CBand::GetNextAvailablePChannel()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	DWORD dwHighestPch = 0;

	CInstrumentListItem* pInstrumentItem;

	POSITION position = m_lstInstruments.GetHeadPosition();
	int nLastPChannel = -1;
	
	while(position)
	{
		pInstrumentItem = m_lstInstruments.GetNext(position);
		ASSERT(pInstrumentItem);
		
		CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
		ASSERT(pInstrument);

		if(nLastPChannel < (int)pInstrument->dwPChannel - 1) // If a vacant position
		{
			return nLastPChannel + 1;						 // Return the last position + 1
		}
		else if(nLastPChannel < (int)pInstrument->dwPChannel)
		{
			nLastPChannel = (int)pInstrument->dwPChannel;
		}
		
		if((int)dwHighestPch < nLastPChannel)
		{
			dwHighestPch = nLastPChannel;
		}
	}
	
	if(nLastPChannel == -1)
	{
		dwHighestPch = nLastPChannel;
	}
	
	return (dwHighestPch + 1);	
}


DWORD CBand::GetLastAvailablePChannel(DWORD nStartIndex)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if((int)nStartIndex == 0)
	{
		return 0;
	}

	CInstrumentListItem* pInstrumentItem;
	POSITION position = m_lstInstruments.GetTailPosition();
	
	int nLastPChannel = (int)nStartIndex - 1;

	while(position)
	{
		pInstrumentItem = m_lstInstruments.GetPrev(position);
		ASSERT(pInstrumentItem);
		
		CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
		ASSERT(pInstrument);

		if(nLastPChannel > (int)pInstrument->dwPChannel + 1) // If a vacant position
		{
			return nLastPChannel - 1;
		}
		else if(nLastPChannel > (int)pInstrument->dwPChannel)
		{
			nLastPChannel = (int)pInstrument->dwPChannel;
		}
	}

	nLastPChannel--;
	nLastPChannel = nLastPChannel < 0 ? nStartIndex : nLastPChannel;
	nLastPChannel = nLastPChannel > 998 ? nStartIndex : nLastPChannel;


	// None before were found so return the start index
	return (nLastPChannel);	
}




void CBand::InsertIntoInstrumentList(CInstrumentListItem* pInstrumentItem)
{

	ASSERT(pInstrumentItem);
	CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
	ASSERT(pInstrument);
	
	POSITION position = m_lstInstruments.GetHeadPosition();

	while(position)
	{
		CInstrumentListItem* pListInstrumentItem = (CInstrumentListItem*)m_lstInstruments.GetNext(position);
		ASSERT(pListInstrumentItem);
		CDMInstrument* pListInstrument = pListInstrumentItem->GetInstrument();
		ASSERT(pListInstrument);

		if(pListInstrument->dwPChannel >= pInstrument->dwPChannel + 1)
		{
			POSITION matchPosition = m_lstInstruments.Find(pListInstrumentItem);
			m_lstInstruments.InsertBefore(matchPosition, pInstrumentItem);
			pInstrument->SendBandUpdate(this);
			return;
		}

	}
	
	m_lstInstruments.AddTail(pInstrumentItem);
	pInstrument->SendBandUpdate(this);
}

CInstrumentListItem* CBand::ChangePChannelNumber(DWORD oldPChannel, DWORD newPChannel)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Do nothing if no change in the PChannel number
	if(oldPChannel == newPChannel)
	{
		return NULL;
	}
	
	CInstrumentListItem* pInstrumentListItem = NULL;
	CDMInstrument* pInstrument = NULL;
		
	POSITION position = m_lstInstruments.GetHeadPosition();
	while(position)
	{
		POSITION oldPosition = position;
		pInstrumentListItem = (CInstrumentListItem*)m_lstInstruments.GetNext(position);
		ASSERT(pInstrumentListItem);
		pInstrument = pInstrumentListItem->GetInstrument();
		ASSERT(pInstrument);

		if(pInstrument->dwPChannel == oldPChannel) 
		{
			m_lstInstruments.RemoveAt(oldPosition);
			break;
		}
	}
	
	if(pInstrumentListItem)
	{
		pInstrument->dwPChannel = newPChannel;
		InsertIntoInstrumentList(pInstrumentListItem);
	}

	return pInstrumentListItem;
}

	
	
CInstrumentListItem* CBand::InsertIntoInstrumentList()
{
	CInstrumentListItem* pInstrumentItem = NULL;
	DWORD dwNextAvailableChannel;

	dwNextAvailableChannel = GetNextAvailablePChannel(); //Start with the last PChannel

	if((int)dwNextAvailableChannel >= m_lstInstruments.GetCount())
	{
		CDMInstrument* pInstrument = new CDMInstrument();
		pInstrument->SetComponent(m_pComponent);
		pInstrument->SetBand(this);
		pInstrument->dwPChannel = dwNextAvailableChannel;
		pInstrument->dwPatch = MAKE_PATCH(pInstrument->IsDrums(), 0, 0, 0);
		memset(pInstrument->dwNoteRanges,0,4 * sizeof(DWORD));
		int nPriorityIndex = pInstrument->dwPChannel%16;
		pInstrument->dwChannelPriority = dwaPriorities[nPriorityIndex];
		pInstrument->nPitchBendRange = 2;
		pInstrumentItem = new CInstrumentListItem(pInstrument);
		m_lstInstruments.AddTail(pInstrumentItem);
		pInstrument->SendBandUpdate(this);
	}
	else if((int)dwNextAvailableChannel > 0)
	{
		POSITION position = m_lstInstruments.FindIndex(dwNextAvailableChannel - 1);
		CDMInstrument* pInstrument = new CDMInstrument();
		pInstrument->SetComponent(m_pComponent);
		pInstrument->SetBand(this);
		pInstrument->dwPChannel = dwNextAvailableChannel;
		pInstrument->dwPatch = MAKE_PATCH(pInstrument->IsDrums(), 0, 0, 0);
		memset(pInstrument->dwNoteRanges,0,4 * sizeof(DWORD));
		int nPriorityIndex = pInstrument->dwPChannel%16;
		pInstrument->dwChannelPriority = dwaPriorities[nPriorityIndex];
		pInstrument->nPitchBendRange = 2;
		pInstrumentItem = new CInstrumentListItem(pInstrument);
		m_lstInstruments.InsertAfter(position, pInstrumentItem);
		pInstrument->SendBandUpdate(this);
	}
	else if((int)dwNextAvailableChannel == 0)
	{
		CDMInstrument* pInstrument = new CDMInstrument();
		pInstrument->SetComponent(m_pComponent);
		pInstrument->SetBand(this);
		pInstrument->dwPChannel = dwNextAvailableChannel;
		pInstrument->dwPatch = MAKE_PATCH(pInstrument->IsDrums(), 0, 0, 0);
		memset(pInstrument->dwNoteRanges,0,4 * sizeof(DWORD));
		int nPriorityIndex = pInstrument->dwPChannel%16;
		pInstrument->dwChannelPriority = dwaPriorities[nPriorityIndex];
		pInstrument->nPitchBendRange = 2;
		pInstrumentItem = new CInstrumentListItem(pInstrument);
		m_lstInstruments.AddHead(pInstrumentItem);
		pInstrument->SendBandUpdate(this);
	}

	return pInstrumentItem;
}

	

int CBand::DeleteFromInstrumentList(DWORD nPChannel)
{
	POSITION position = m_lstInstruments.FindIndex(nPChannel);
	
	if(position)
	{
		CInstrumentListItem* pInstrumentItem = m_lstInstruments.GetAt(position);
		ASSERT(pInstrumentItem);
		CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
		ASSERT(pInstrument);

		m_lstInstruments.RemoveAt(position);
		pInstrument->SendBandUpdate(this);

		if(pInstrumentItem->Release() == 0)
			pInstrumentItem = NULL;
	}

	return m_lstInstruments.GetCount(); // Return the number of instruments in the list
}

	

int CBand::DeleteFromInstrumentList(CInstrumentListItem* pInstrumentItem)
{
	ASSERT(pInstrumentItem);

	POSITION position = m_lstInstruments.Find(pInstrumentItem);

	if(position)
	{
		m_lstInstruments.RemoveAt(position);
		
		CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
		ASSERT(pInstrument);
		pInstrument->SendBandUpdate(this);

		if(pInstrumentItem->Release() == 0)
			pInstrumentItem = NULL;
	}

	return m_lstInstruments.GetCount(); // Return the number of instruments in the list
}

bool CBand::IsPChannelFree(DWORD dwPChannel)
{
	// Go throught he instrument list and find out if this PChannel is used...
	POSITION position = m_lstInstruments.GetHeadPosition();
	while(position)
	{
		CInstrumentListItem* pInstrumentItem = (CInstrumentListItem*) m_lstInstruments.GetNext(position);
		ASSERT(pInstrumentItem);
		if(pInstrumentItem)
		{
			CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
			ASSERT(pInstrument);
			if(pInstrument && pInstrument->dwPChannel == dwPChannel)
			{
				return false;
			}
		}
	}

	return true;

}


/////////////////////////////////////////////////////////////////////////////
// CBand::Load_IMA_Band

HRESULT CBand::Load_IMA_Band( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ioBand      iBand;
    IStream*    pIStream;
    HRESULT     hr = E_FAIL;
    MMCKINFO    ck;
	DWORD		cb;
	DWORD		cSize;
    char        sz[32];
	int         i;
	int			iTrack;
	CString		cs_Collection;
	IDMUSProdReferenceNode*		pCollectionRef=NULL;

    pIStream = pIRiffStream->GetStream();
    ASSERT( pIStream != NULL );

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
    {
        switch( ck.ckid )
		{
	        case FOURCC_BAND:
				cSize = min( ck.cksize, sizeof( iBand ) );
				hr = pIStream->Read( &iBand, cSize, &cb );
				if( FAILED( hr ) || cb != cSize )
				{
					hr = E_FAIL;
					goto ON_ERR;
				}

				SetModifiedFlag( FALSE );
				WideCharToMultiByte( CP_ACP, 0, iBand.wstrName, -1, sz, sizeof( sz ), NULL, NULL );
				m_csName = sz;
				m_strDefaultCollection = iBand.szCollection;
				m_fDefault = iBand.fDefault;
				
				if ( !m_strDefaultCollection.IsEmpty() )
				{
					pCollectionRef = m_pComponent->FindDLSCollection(m_strDefaultCollection, pIStream);
				}

				for( i = 0 ; i < 16 ; ++i )
				{
					iTrack = IMA25_VOICEID_TO_PCHANNEL(i);
					CDMInstrument* pInstrument = GetInstrumentFromList(iTrack);
					ASSERT(pInstrument);
					//memset( &m_aInstr[iTrack], 0, sizeof( DMUS_IO_INSTRUMENT ) );
					memset( pInstrument, 0, sizeof( DMUS_IO_INSTRUMENT ) );
					// 5/21/98 JHD: Rewrote to accurately reflect how DirectMusic imports IMA instruments
					if(iBand.awDLSBank[i] & 0x8000) 
					{
						// We have a plain old GM collection where MSB & LSB are both zero
						pInstrument->dwPatch = 0;
						pInstrument->dwPatch |= (iBand.abPatch[i] & 0x7F);
						pInstrument->dwFlags |= (DMUS_IO_INST_GM | DMUS_IO_INST_GS);
					}
					else
					{
						if(iBand.awDLSBank[i] & 0x4000)
						{
							// We has a GS collection with valid MSB and LSB numbers
							pInstrument->dwPatch = 0;
							pInstrument->dwPatch |= (iBand.abDLSPatch[i] & 0x7F);
							pInstrument->dwPatch |= (iBand.awDLSBank[i] & 0x7F) << 8; // Set LSB
							pInstrument->dwPatch |= ((iBand.awDLSBank[i] >> 7) & 0x7F) << 16; // Set MSB
							pInstrument->dwFlags |= (DMUS_IO_INST_BANKSELECT | DMUS_IO_INST_GS | DMUS_IO_INST_GM);
						}
						else
						{
							if( !pCollectionRef )
							{
								// We have no unique DLS file so we will assume GM
								pInstrument->dwPatch = 0;
								pInstrument->dwPatch |= (iBand.abPatch[i] & 0x7F);
								pInstrument->dwFlags |= (DMUS_IO_INST_GM | DMUS_IO_INST_GS);
							}
							else
							{
								// We have a unique DLS file
								pInstrument->dwPatch = 0;
								pInstrument->dwPatch |= (iBand.abDLSPatch[i] & 0x7F);
								pInstrument->dwPatch |= (iBand.awDLSBank[i] & 0x7F) << 8; // Set LSB
								pInstrument->dwPatch |= ((iBand.awDLSBank[i] >> 7) & 0x7F) << 16; // Set MSB
								pInstrument->dwFlags |= (DMUS_IO_INST_BANKSELECT);
								pInstrument->SetDLSCollection( pCollectionRef );
							}
						}
					}
					
					pInstrument->bPan = iBand.abPan[i];
					pInstrument->bVolume = iBand.abVolume[i];
					pInstrument->dwPChannel = iTrack;
					EnablePatch( iTrack );
					
					if(iBand.achOctave[i] != 0)
					{
						pInstrument->nTranspose = iBand.achOctave[i];
					}

					// Set drum-kit bit if a drum-kit
					if(pInstrument->dwPChannel % 16 == DRUM_PCHANNEL)
					{
						pInstrument->dwPatch |= 0x80000000;
					}
				}
				hr = S_OK;
				if (pCollectionRef)
				{
					pCollectionRef->Release();
					pCollectionRef = NULL;
				}
				break;
		}
        pIRiffStream->Ascend( &ck, 0 );
    }
ON_ERR:
    pIStream->Release();
    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CBand::LoadDMBand

HRESULT CBand::LoadDMBand( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    IStream*    pIStream;
    HRESULT     hr = E_FAIL;
    MMCKINFO    ck;
    MMCKINFO    ckList;
	DWORD		cb;

    pIStream = pIRiffStream->GetStream();
    ASSERT( pIStream != NULL );

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
    {
        switch( ck.ckid )
		{
			case FOURCC_LIST:
				switch ( ck.fccType )
				{
					case DMUS_FOURCC_INSTRUMENTS_LIST:
						if(FAILED(LoadDMInstruments(pIRiffStream, &ck)))
						{
							pIStream->Release();
							return E_FAIL;
						}
						break;
					case DMUS_FOURCC_INFO_LIST:
						{
							CInfo infoList;
							infoList.Load(pIRiffStream, &ck);
							m_csName = infoList.m_csName;
						}
						break;
					case DMUS_FOURCC_UNFO_LIST:
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case RIFFINFO_INAM:
								case DMUS_FOURCC_UNAM_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_csName );
									break;
							}
					        pIRiffStream->Ascend( &ckList, 0 );
						}
						break;
				}
				break;
			case DMUS_FOURCC_GUID_CHUNK: //  <guid-ck>
				if( ck.cksize >= sizeof(GUID) )
				{
					hr = pIStream->Read(&m_guidBand, sizeof(GUID), &cb);
					if ( FAILED(hr) || cb != sizeof(GUID) )
					{
						pIStream->Release();
						return hr;
					}
				}
				break;				
			case DMUS_FOURCC_VERSION_CHUNK:
				if( ck.cksize >= sizeof(DMUS_IO_VERSION) )
				{
					hr = pIStream->Read(&m_Version, sizeof(DMUS_IO_VERSION), &cb);
					if ( FAILED(hr) || cb != sizeof(GUID) )
					{
						pIStream->Release();
						return hr;
					}
				}
				break;
		}
		if (pIRiffStream->Ascend( &ck,0 ) != 0)
		{
			pIStream->Release();
			return E_FAIL;
		}
	}

	pIStream->Release();
	return S_OK;
}

HRESULT CBand::LoadDMInstruments(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Clear the default instrument list before loading the band data
	// We don't do this for IMA bands 'cause they will always have 16 channels
	DeleteInstrumentList();

    IStream*    pIStream;
    HRESULT     hr = E_FAIL;
	MMCKINFO    ckInst;

    pIStream = pIRiffStream->GetStream();
    ASSERT( pIStream != NULL );

	ckInst.fccType = DMUS_FOURCC_INSTRUMENT_LIST;

	while ( pIRiffStream->Descend(&ckInst, pckMain, MMIO_FINDLIST) == 0 )
	{
		CDMInstrument *pInstr = new CDMInstrument();

		pInstr->SetComponent( m_pComponent );
		pInstr->SetBand( this );
		if(FAILED(pInstr->Load(pIRiffStream, &ckInst)))
		{
			pIStream->Release();
			return E_FAIL;
		}

		// Always true
		//if( pInstr->dwPChannel >= 0 )
		{
			// If the priority is 0 then correct it to standard....(old content did not have priorities saved out)
			if(pInstr->dwChannelPriority == 0)
			{
				int nPriorityIndex = pInstr->dwPChannel%16;
				pInstr->dwChannelPriority = dwaPriorities[nPriorityIndex];
			}

			CInstrumentListItem* pInstrumentItem = new CInstrumentListItem(pInstr);
			InsertIntoInstrumentList(pInstrumentItem);
		}

		pIRiffStream->Ascend( &ckInst, 0 );
	}

	pIStream->Release();
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand::SaveBandUnfoList

HRESULT CBand::SaveBandUnfoList( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
    MMCKINFO ckMain;
    MMCKINFO ck;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write UNFO LIST header
	ckMain.fccType = DMUS_FOURCC_UNFO_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write Band name
	{
		ck.ckid = DMUS_FOURCC_UNAM_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_csName );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	if( pIRiffStream->Ascend(&ckMain, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    if( pIStream )
	{
		pIStream->Release();
	}
    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CBand::SaveBandChunk

HRESULT CBand::SaveBandChunk( IDMUSProdRIFFStream* pIRiffStream )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    IStream* pIStream;
    HRESULT hr;
    MMCKINFO ckMain;
	MMCKINFO ck;
	DWORD cb;
	DMUSProdStreamInfo	StreamInfo;

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	FileType ftFileType = FT_RUNTIME;
	GUID guidDataFormat = GUID_CurrentVersion;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		ftFileType = StreamInfo.ftFileType;
		guidDataFormat= StreamInfo.guidDataFormat;
		pPersistInfo->Release();
	}

    hr = E_FAIL;

	// save <guid-ck>
	ck.ckid = DMUS_FOURCC_GUID_CHUNK;
	if (pIRiffStream->CreateChunk(&ck, 0) == 0 )
	{
		hr = pIStream->Write(&m_guidBand, sizeof(GUID), &cb);
		if ( FAILED(hr) || cb  != sizeof(GUID) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}
	if ( pIRiffStream->Ascend(&ck,0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}
	
	// Save Band UNFO LIST header
	hr = SaveBandUnfoList( pIRiffStream );
	if( FAILED( hr ) )
	{
		goto ON_ERROR;
	}

	// Save instrument list
    ckMain.fccType = DMUS_FOURCC_INSTRUMENTS_LIST;
    if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATELIST ) == 0 )
    {
		int nNumberOfInstruments = m_lstInstruments.GetCount();
		for( int i = 0 ; i < nNumberOfInstruments ; ++i )
		{	//for each instrument
			CDMInstrument* pInstrument = GetInstrumentByIndex(i);
			ASSERT(pInstrument);

			hr = pInstrument->Save(pIRiffStream);
			if (FAILED(hr))
			{
				goto ON_ERROR;
			}
		}//for each instrument
		if ( pIRiffStream->Ascend( &ckMain, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
    }

ON_ERROR:
    if( pIStream )
	{
		pIStream->Release();
	}
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBand::ReadListInfoFromStream

HRESULT CBand::ReadListInfoFromStream( IStream* pIStream, DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;
	MMCKINFO ck;
	DWORD dwSize;
	DWORD dwByteCount;
	CString strName;
	CString strDescriptor;
	GUID guidBand;

	ASSERT( pIStream != NULL );
    ASSERT( pListInfo != NULL );

	if( pIStream == NULL
	||  pListInfo == NULL )
	{
		return E_INVALIDARG;
	}

	memset( &guidBand, 0, sizeof(GUID) );

	if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		ckMain.fccType = DMUS_FOURCC_BAND_FORM;

		if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
		{
			// Get Band GUID
			ck.ckid = DMUS_FOURCC_GUID_CHUNK;

			if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDCHUNK ) == 0 )
			{
				dwSize = min( ck.cksize, sizeof( GUID ) );
				if( FAILED ( pIStream->Read( &guidBand, dwSize, &dwByteCount ) )
				||  dwByteCount != dwSize )
				{
					memset( &guidBand, 0, sizeof(GUID) );
				}
			}

			// Get Band name
			ck.fccType = DMUS_FOURCC_UNFO_LIST;

			if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDLIST ) == 0 )
			{
				DWORD dwPosName = StreamTell( pIStream );
			
				ck.ckid = DMUS_FOURCC_UNAM_CHUNK;
				if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDCHUNK ) == 0 )
				{
					ReadMBSfromWCS( pIStream, ck.cksize, &strName );
				}
				else
				{
				    StreamSeek( pIStream, dwPosName, STREAM_SEEK_SET );

					ck.ckid = RIFFINFO_INAM;
					if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDCHUNK ) == 0 )
					{
						ReadMBSfromWCS( pIStream, ck.cksize, &strName );
					}
				}
			}
		}

		RELEASE( pIRiffStream );
	}

	if( !strName.IsEmpty() )
	{
		pListInfo->bstrName = strName.AllocSysString();
		pListInfo->bstrDescriptor = strDescriptor.AllocSysString();
		memcpy( &pListInfo->guidObject, &guidBand, sizeof(GUID) );

		// Must check pListInfo->wSize before populating additional fields
		return S_OK;
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CBand::GetGUID

void CBand::GetGUID( GUID* pguidBand )
{
	if( pguidBand )
	{
		*pguidBand = m_guidBand;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CBand::SetGUID

void CBand::SetGUID( GUID guidBand )
{
	m_guidBand = guidBand;
	SetModifiedFlag( TRUE );

	// Notify connected nodes that Band GUID has changed
	theApp.m_pIFramework->NotifyNodes( this, DOCROOT_GuidChange, NULL );
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdPropPageObject::GetData

HRESULT CBand::GetData( void** ppData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppData == NULL )
	{
		return E_POINTER;
	}

	*ppData = this;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdPropPageObject::SetData

HRESULT CBand::SetData( void* pData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}


HRESULT CBand::OnViewProperties()
{
	ASSERT( m_pComponent->m_pIFramework != NULL );

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


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdPropPageObject::OnShowProperties

HRESULT CBand::OnShowProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pIFramework != NULL );

	CDllBasePropPageManager* pPageManager;

	// Get the Band page manager
	if( theApp.m_pIPageManager
	&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_BandPropPageManager ) == S_OK )
	{
		pPageManager = (CDllBasePropPageManager *)theApp.m_pIPageManager;
	}
	else
	{
		pPageManager = new CBandPropPageManager();
	}
	if( pPageManager == NULL )
	{
		return E_FAIL;
	}

	// Show the Band properties
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		if( SUCCEEDED ( pIPropSheet->SetPageManager(pPageManager) ) )
		{
			theApp.m_pIPageManager = pPageManager;
			pPageManager->SetObject( this );
		}

		pIPropSheet->Show( TRUE );
		pIPropSheet->Release();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdPropPageObject::OnRemoveFromPageManager

HRESULT CBand::OnRemoveFromPageManager( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdGetReferencedNodes implementation

/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdGetReferencedNodes::GetReferencedNodes

HRESULT CBand::GetReferencedNodes( DWORD *pdwArraySize, IDMUSProdNode **ppIDMUSProdNode, DWORD dwErrorLength, WCHAR *wcstrErrorText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pdwArraySize == NULL )
	{
		return E_POINTER;
	}

    bool fAddedErrorHeader = false;

	// Iterate through the list of instruments
	DWORD dwIndex = 0;
	POSITION position = m_lstInstruments.GetHeadPosition();
	while(position)
	{
		// Get the instrument from east list item
		CDMInstrument* pInstrument = m_lstInstruments.GetNext(position)->GetInstrument();
		ASSERT(pInstrument);

		// If the instrument has a patch change
		if( pInstrument && (pInstrument->dwFlags & DMUS_IO_INST_PATCH) )
		{
			const DWORD dwOldIndex = dwIndex;

			// If the instrument has a DLS reference
			if( pInstrument->m_pDMReference)
			{
				// Get the node underlying the reference
				IDMUSProdNode *pNode;
				if( SUCCEEDED( pInstrument->m_pDMReference->GetReferencedFile( &pNode ) ) )
				{
					// If we have an array, and we haven't run out of room
					if( ppIDMUSProdNode
					&&	(*pdwArraySize) > dwIndex )
					{
						// Add the node to the array
						ppIDMUSProdNode[dwIndex] = pNode;
						ppIDMUSProdNode[dwIndex]->AddRef();
					}

					// Increment the index
					dwIndex++;

					// Release the node
					pNode->Release();
				}
			}

			// If we didn't find the referenced DLS collection, and there is space in the error string
			if( (dwOldIndex == dwIndex)
			&&	dwErrorLength
			&&	wcstrErrorText
			&&	(wcslen(wcstrErrorText) < dwErrorLength) )
			{
				// Warn the user that this instrument will fail to load on the Xbox

				// Allocate a temporary array to store the current error string
				const int nTempLen = dwErrorLength - wcslen(wcstrErrorText);
				WCHAR *wcstrTmp = new WCHAR[nTempLen + 1];
				if( wcstrTmp )
				{
					// Zero out our temp. string
					ZeroMemory( wcstrTmp, sizeof(WCHAR) * (nTempLen + 1) );

                    // The number of wide characters written
                    int nWritten = 0;

                    if( !fAddedErrorHeader )
                    {
                        fAddedErrorHeader = true;

                        // Add the error header
					    CString strErrorText;
                        AfxFormatString1( strErrorText, IDS_XBOX_ERROR_NO_COLLECTION, m_csName );

					    // Convert to wide characters
					    nWritten = MultiByteToWideChar( CP_ACP, 0, strErrorText, -1, wcstrTmp, nTempLen );

					    // Append the error text
					    wcscat( wcstrErrorText, wcstrTmp );

					    // Zero out our temp. string again
					    ZeroMemory( wcstrTmp, sizeof(WCHAR) * (nTempLen + 1) );
                    }

                    // If there is space left in the string
                    if( nTempLen - nWritten > 0 )
                    {
					    // Write out the error string
					    CString strErrorText;
					    strErrorText.Format( IDS_XBOX_ERROR_PATCH, pInstrument->dwPChannel, MSB(pInstrument->dwPatch), LSB(pInstrument->dwPatch), pInstrument->dwPatch & 0x7F );

					    // Convert to wide characters
					    MultiByteToWideChar( CP_ACP, 0, strErrorText, -1, wcstrTmp, nTempLen - nWritten );

					    // Append the error text
					    wcscat( wcstrErrorText, wcstrTmp );
                    }

					delete []wcstrTmp;
				}
			}
		}
	}

	HRESULT hr = ppIDMUSProdNode && ((*pdwArraySize) < dwIndex) ? S_FALSE : S_OK;

	// Store the number of nodes we returned (or that we require)
	*pdwArraySize = dwIndex;

	return hr;
}


HRESULT CBand::SyncAllInstruments(DMUS_PMSGT_TYPES pmsgType)
{
	POSITION position = m_lstInstruments.GetHeadPosition();
	while(position)
	{
		CInstrumentListItem* pInstrumentItem = (CInstrumentListItem*)m_lstInstruments.GetNext(position);
		ASSERT(pInstrumentItem);
		if(pInstrumentItem)
		{
			CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
			ASSERT(pInstrument);
			if(pInstrument)
			{
				if(FAILED(pInstrument->Send(this, pmsgType)))
				{
					return E_FAIL;
				}
			}
		}
	}
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////
// CDMInstrument implementation
//

// Init the static m_bSendBandUpdate flag
BOOL CDMInstrument::m_bSendBandUpdate = TRUE;

CDMInstrument::CDMInstrument() 
{
	m_fSelected = false;
	m_pDMReference = NULL;
	m_pComponent = NULL;

	ResetInstruemnt();
}

CDMInstrument::~CDMInstrument()
{
	SetDLSCollection( NULL );
}

void CDMInstrument::ResetInstruemnt()
{
	m_fSelected = false;
	SetDLSCollection( NULL );

	dwPatch = 0;
	dwAssignPatch = 0;
	ZeroMemory( dwNoteRanges, sizeof(DWORD) * 4 );
	dwPChannel = 0;
	dwFlags = DMUS_IO_INST_GM;
	bPan = 64;
	bVolume = 64;
	nTranspose = 0; // Balanced
	dwChannelPriority = DAUD_STANDARD_VOICE_PRIORITY; 
	nPitchBendRange = 2;
}

CDMInstrument& CDMInstrument::operator = (const CDMInstrument& dmInstrument)
{
	if(&dmInstrument == this)
	{
		return *this;
	}

	*dynamic_cast<DMUS_IO_INSTRUMENT*>(const_cast<CDMInstrument*>(this)) = dmInstrument;

	m_csCollection = dmInstrument.m_csCollection;
	if( m_pComponent != dmInstrument.m_pComponent )
	{
		if( m_pComponent )
		{
			m_pComponent->Release();
		}
		m_pComponent = dmInstrument.m_pComponent;
		if( m_pComponent )
		{
			m_pComponent->AddRef();
		}
	}

	SetDLSCollection( dmInstrument.m_pDMReference );

	return *this;
}

bool CDMInstrument::IsDrums()
{
	return (dwPChannel == 9 || dwPChannel%16 == 9);
}

HRESULT CDMInstrument::Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain)
{
    IStream*    pIStream;
	MMCKINFO    ck;
    HRESULT     hr = E_FAIL;
	DWORD		dwByteCount;
	DWORD		dwCurrentFilePos;
	DWORD		dwSize;
	IDMUSProdFileRefChunk*	pIFileRef = NULL;
	IDMUSProdNode*			pIDocRoot = NULL;

    pIStream = pIRiffStream->GetStream();
    ASSERT( pIStream != NULL );

	dwCurrentFilePos = StreamTell( pIStream );

	while ( pIRiffStream->Descend(&ck, pckMain, 0) == 0 )
	{
		switch( ck.ckid )
		{
			case DMUS_FOURCC_INSTRUMENT_CHUNK:
			{
				dwSize = min( ck.cksize, sizeof(DMUS_IO_INSTRUMENT) );
				hr = pIStream->Read( dynamic_cast<DMUS_IO_INSTRUMENT*>(this), dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
				break;
			}

			case FOURCC_INST_DESIGNTIME:
			{
				// Read the selection flag
				hr = pIStream->Read(&m_fSelected, sizeof(bool), &dwByteCount);
				if(FAILED(hr) || dwByteCount != sizeof(bool))
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
				break;
			}

			case FOURCC_DMUSPROD_FILEREF:
			{
				StreamSeek( pIStream, dwCurrentFilePos, 0 );
				hr = m_pComponent->m_pIFramework->QueryInterface( IID_IDMUSProdFileRefChunk, (void**) &pIFileRef );
				if( FAILED ( hr ) )
				{
					goto ON_ERROR;
				}
				if( SUCCEEDED ( pIFileRef->LoadRefChunk( pIStream, &pIDocRoot ) ) )
				{
					IDMUSProdReferenceNode* pCollectionRef;

					// Create and store reference node for DLS Collection
					pCollectionRef = m_pComponent->CreateCollectionRefNode( pIDocRoot );
					if( pCollectionRef )
					{
						SetDLSCollection( pCollectionRef );
						pCollectionRef->Release();

						dwFlags |= (DMUS_IO_INST_BANKSELECT);
						dwFlags &= ~(DMUS_IO_INST_GS | DMUS_IO_INST_GM);
					}
					pIDocRoot->Release();
				}
				break;
			}

			case FOURCC_LIST:
			{
				if( ck.fccType == DMUS_FOURCC_REF_LIST )
				{
					MMCKINFO ckName;

					ckName.ckid = DMUS_FOURCC_NAME_CHUNK;
					if( pIRiffStream->Descend( &ckName, NULL, MMIO_FINDCHUNK ) == 0 )
					{
						// Store DLS Collection name
						ReadMBSfromWCS( pIStream, ckName.cksize, &m_csCollection );
					}
				}
				break;
			}
		}

		pIRiffStream->Ascend( &ck, 0 );
		dwCurrentFilePos = StreamTell( pIStream );
	}

	if( m_pDMReference == NULL )
	{
		// Do we have a DLS Collection name?
		if( !m_csCollection.IsEmpty() )
		{
			IDMUSProdReferenceNode* pCollectionRef;

			// Framework could not resolve DLS Collection file reference
			// so we will ask user to help
			pCollectionRef = m_pComponent->FindDLSCollection( m_csCollection, pIStream );
			if( pCollectionRef )
			{
				SetDLSCollection( pCollectionRef );
				pCollectionRef->Release();
			}
			else
			{
				hr = S_FALSE;
				//goto ON_ERROR;
			}
		}
	}

	if( m_pDMReference )
	{
		// Reset Instrument's Collection name
		IDMUSProdNode* pIDocRootNode;

		if( SUCCEEDED (	m_pDMReference->GetReferencedFile( &pIDocRootNode ) ) )
		{
			BSTR bstrCollectionName;

			pIDocRootNode->GetNodeName( &bstrCollectionName );
			m_csCollection = bstrCollectionName;
			::SysFreeString( bstrCollectionName );

			pIDocRootNode->Release();
		}
	}

ON_ERROR:
	RELEASE( pIStream );
	RELEASE( pIFileRef );
	return hr;
}

HRESULT CDMInstrument::Save(IDMUSProdRIFFStream* pIRiffStream)
{
	ASSERT(m_pBand);
	if(m_pBand == NULL)
		return E_FAIL;

    IStream*    pIStream;
    HRESULT     hr = E_FAIL;
	MMCKINFO ckInst;
	MMCKINFO ck;
	FileType ftFileType = FT_RUNTIME;
	GUID guidDataFormat = GUID_CurrentVersion;
	IDMUSProdPersistInfo* pPersistInfo;
	DMUSProdStreamInfo	StreamInfo;

	DWORD		cb;

    pIStream = pIRiffStream->GetStream();
    ASSERT( pIStream != NULL );

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		ftFileType = StreamInfo.ftFileType;
		guidDataFormat= StreamInfo.guidDataFormat;
		pPersistInfo->Release();
	}

	
	ckInst.fccType = DMUS_FOURCC_INSTRUMENT_LIST;
	if( pIRiffStream->CreateChunk( &ckInst, MMIO_CREATELIST ) == 0 )
	{
		ck.ckid = DMUS_FOURCC_INSTRUMENT_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) == 0 )
		{
			hr = pIStream->Write(dynamic_cast<DMUS_IO_INSTRUMENT*>(this), sizeof(DMUS_IO_INSTRUMENT), &cb);
			if ( cb != sizeof(DMUS_IO_INSTRUMENT) )
			{
				hr = E_FAIL;
			}
			if ( pIRiffStream->Ascend( &ck, 0 ) != 0 )
			{
				hr = E_FAIL;
			}
		}
		if(m_fSelected && m_pBand->m_bInUndoRedo == true && ftFileType == FT_DESIGN)
		{
			ck.ckid = FOURCC_INST_DESIGNTIME;
			if(pIRiffStream->CreateChunk(&ck, 0) == 0)
			{
				hr = pIStream->Write((LPSTR)&(m_fSelected), sizeof(bool), &cb);
				if(cb != sizeof(bool))
				{
					hr = E_FAIL;
				}
				if(pIRiffStream->Ascend(&ck, 0) != 0)
				{
					hr = E_FAIL;
				}
			}
		}
		

		if (m_pDMReference)
		{
			if( ::IsEqualGUID( guidDataFormat, GUID_DirectMusicObject ))
			{
				hr = SaveDMRef( pIRiffStream, WL_PRODUCER );
			}
			else if( ::IsEqualGUID( guidDataFormat, GUID_CurrentVersion ))
			{
				hr = SaveDMRef( pIRiffStream, WL_DIRECTMUSIC );
				if (FAILED(hr))
				{
					pIStream->Release();
					return hr;
				}
				if (ftFileType == FT_DESIGN)
				{
					hr = SaveFileRef(pIRiffStream);
					if (FAILED(hr))
					{
						pIStream->Release();
						return hr;
					}
				}
			}
		}
		if ( pIRiffStream->Ascend( &ckInst, 0 ) != 0 )
		{
			hr = E_FAIL;
		}
	}
	pIStream->Release();
	return hr;
}

HRESULT CDMInstrument::SaveDMRef( IDMUSProdRIFFStream* pIRiffStream, WhichLoader whichLoader )
{
	IDMUSProdLoaderRefChunk*	pRefChunkLoader=NULL;
	IDMUSProdNode*				pIDocRootNode;
	IStream*					pIStream;

	// Get the DocRoot node
	if( FAILED ( m_pDMReference->GetReferencedFile( &pIDocRootNode ) ) )
	{
		return E_FAIL;
	}
	if( pIDocRootNode == NULL )
	{
		return S_OK;
	}

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	if( SUCCEEDED(m_pComponent->m_pIFramework->QueryInterface(IID_IDMUSProdLoaderRefChunk, (void**)&pRefChunkLoader)) )
	{
		if( pRefChunkLoader )
		{
			switch( whichLoader )
			{
				case WL_PRODUCER:
					pRefChunkLoader->SaveRefChunkForLoader( pIStream,
															pIDocRootNode,
															CLSID_DirectMusicCollection,
															NULL,
															whichLoader );
					break;

				case WL_DIRECTMUSIC:
				{
					// Prepare the DMUS_OBJECTDESC structure
					DMUS_OBJECTDESC dmusObjectDesc;

					IDLSQueryInstruments *pDLSQuery;
					if( SUCCEEDED( m_pDMReference->QueryInterface( IID_IDLSQueryInstruments, (void **)&pDLSQuery ) ) )
					{
						pDLSQuery->GetObjectDescriptor( sizeof(DMUS_OBJECTDESC), &dmusObjectDesc );
						pDLSQuery->Release();
					}
					else
					{
						memset( &dmusObjectDesc, 0, sizeof(DMUS_OBJECTDESC) );
						dmusObjectDesc.dwSize = sizeof(DMUS_OBJECTDESC);

						dmusObjectDesc.dwValidData = (DMUS_OBJ_CLASS | DMUS_OBJ_NAME);
						
						dmusObjectDesc.guidClass = CLSID_DirectMusicCollection;
						MultiByteToWideChar( CP_ACP, 0, m_csCollection, -1, 
							dmusObjectDesc.wszName, sizeof( dmusObjectDesc.wszName ) / sizeof( wchar_t ) );
					}

					pRefChunkLoader->SaveRefChunkForLoader( pIStream,
															pIDocRootNode,
															CLSID_DirectMusicCollection,
															&dmusObjectDesc,
															whichLoader );
					break;
				}
			}
			pRefChunkLoader->Release();
		}
	}

	pIDocRootNode->Release();
	pIStream->Release();
	return S_OK;
}

HRESULT CDMInstrument::SaveFileRef(IDMUSProdRIFFStream* pIRiffStream)
{
	IDMUSProdFileRefChunk* pIFileRefChunk;
	IStream*			   pIStream;
	IDMUSProdNode*		   pIDocRootNode;

	// Get the DocRoot node
	if( FAILED ( m_pDMReference->GetReferencedFile( &pIDocRootNode ) ) )
	{
		return E_FAIL;
	}
	if( pIDocRootNode == NULL )
	{
		return S_OK;
	}

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	if( SUCCEEDED(m_pComponent->m_pIFramework->QueryInterface(IID_IDMUSProdFileRefChunk, (void**)&pIFileRefChunk)) )
	{
		pIFileRefChunk->SaveRefChunk( pIStream, pIDocRootNode );
		pIFileRefChunk->Release();
	}

	pIDocRootNode->Release();
	pIStream->Release();
	return S_OK;
}

BOOL CDMInstrument::SendBandUpdate(IDMUSProdNode* pIBandNode)
{
	ASSERT(pIBandNode);

	BOOL fSendChange = TRUE;
	IDMUSProdNode* pIDocRootNode = NULL;

	MUSIC_TIME mtTime;

	if( SUCCEEDED ( pIBandNode->GetDocRootNode(&pIDocRootNode) ) )
	{
		if( pIDocRootNode != pIBandNode )
		{
			IDMUSProdNotifySink* pINotifySink;

			if( SUCCEEDED ( pIDocRootNode->QueryInterface(IID_IDMUSProdNotifySink, (void**)&pINotifySink) ) )
			{
				if( pINotifySink->OnUpdate( pIBandNode, GUID_BAND_ChangeNotifyMsg, NULL ) != S_OK ) // Ask DocRoot if change should be sent
				{
					fSendChange = FALSE;

					if( SUCCEEDED( m_pComponent->m_pIDMPerformance->GetTime( NULL, &mtTime) ) )
					{
					   IDirectMusicSegmentState* pSegmentState = NULL;
					   HRESULT hr = m_pComponent->m_pIDMPerformance->GetSegmentState( &pSegmentState, mtTime );
					   if( hr == DMUS_E_NOT_FOUND )
					   {
						  fSendChange = TRUE;
					   }
					   if(pSegmentState)
						   pSegmentState->Release();
					}
				}

				pINotifySink->Release();
			}
	
		}

		pIDocRootNode->Release();
	}

	return fSendChange;
}

HRESULT CDMInstrument::Send(IDMUSProdNode* pIBandNode, UINT uMsgType)
{
	ASSERT( pIBandNode != NULL );
	ASSERT(m_pComponent);

	static BOOL fSendChange = TRUE;
	
	if(m_bSendBandUpdate)
	{
		// We need to send band changes only once
		m_bSendBandUpdate = FALSE;

		fSendChange = SendBandUpdate(pIBandNode);
		if( fSendChange == FALSE)
		{
			// No need to send the change
			return S_OK;
		}
	}

	if(fSendChange == FALSE || uMsgType == NULL)
	{
		// No need to send the change
		return S_OK;
	}

	ASSERT( m_pComponent->m_pIDMPerformance != NULL );
	if ( m_pComponent->m_pIDMPerformance == NULL )
	{
		return E_FAIL;
	}

	HRESULT hr;
	IDirectMusicGraph *pDMGraph = NULL;
	
	hr = m_pComponent->m_pIDMPerformance->QueryInterface( IID_IDirectMusicGraph, (void**) &pDMGraph );
	if ( FAILED( hr ) )
	{
		return E_FAIL;
	}
	switch (uMsgType)
	{
		case DM_PATCH_CHANGE:
		{
			DMUS_PATCH_PMSG* pMsg = NULL;

			hr = m_pComponent->m_pIDMPerformance->AllocPMsg(sizeof(DMUS_PATCH_PMSG), (DMUS_PMSG**)&pMsg);
			if (FAILED(hr))
			{
				pDMGraph->Release();
				return S_OK;
			}
			
			ZeroMemory(pMsg, sizeof(DMUS_PATCH_PMSG));
			pMsg->dwSize = sizeof(DMUS_PATCH_PMSG);
			pMsg->byInstrument = (BYTE) (dwPatch & 0x7F);
			pMsg->byMSB = (BYTE)MSB(dwPatch);
			pMsg->byLSB = (BYTE)LSB(dwPatch);

			// DMUS_PMSG members that need to be initialized
			pMsg->dwFlags |= DMUS_PMSGF_REFTIME;
			pMsg->dwPChannel = dwPChannel;
			pMsg->dwType = DMUS_PMSGT_PATCH;

			hr = m_pComponent->SendPMsg((DMUS_PMSG*)pMsg, m_pBand ? m_pBand->m_pAudiopath : NULL);

		}
		break;
		case DM_TRANSPOSE:
		{
			DMUS_TRANSPOSE_PMSG *pMsg = NULL;
			hr = m_pComponent->m_pIDMPerformance->AllocPMsg(sizeof(DMUS_TRANSPOSE_PMSG), (DMUS_PMSG**)&pMsg);
			if(SUCCEEDED(hr))
			{
				ZeroMemory(pMsg, sizeof(DMUS_TRANSPOSE_PMSG));
				pMsg->dwSize = sizeof(DMUS_TRANSPOSE_PMSG);

				// DMUS_TRANSPOSE_PMSG members that need to be initialized 
				pMsg->nTranspose = nTranspose;
				// DMUS_PMSG members that need to be initialized
				
				pMsg->dwFlags |= DMUS_PMSGF_REFTIME;
				pMsg->dwPChannel = dwPChannel;
				//pMsg->dwVirtualTrackID = smsg.m_dwVirtualTrackID;
				pMsg->dwType = DMUS_PMSGT_TRANSPOSE;

				// Initializes a few other DMUS_PMSG members
				hr = pDMGraph->StampPMsg((DMUS_PMSG*)pMsg);
				
				hr = m_pComponent->SendPMsg((DMUS_PMSG*)pMsg, m_pBand ? m_pBand->m_pAudiopath : NULL);
			}
		}
		break;
		case DM_VOLUME:
		{
			DMUS_MIDI_PMSG* pMsg = NULL;
			hr = m_pComponent->m_pIDMPerformance->AllocPMsg(sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG**)&pMsg);

			if(SUCCEEDED(hr))
			{
				ZeroMemory(pMsg, sizeof(DMUS_MIDI_PMSG));
				pMsg->dwSize = sizeof(DMUS_MIDI_PMSG);
				// DMUS_MIDI_PMSG members that need to be initialized 
				pMsg->bStatus = MIDI_CONTROL_CHANGE;
				pMsg->bByte1 = MIDI_CC_VOLUME;
				pMsg->bByte2 = bVolume;
				// DMUS_PMSG members that need to be initialized
				pMsg->dwFlags |= DMUS_PMSGF_REFTIME;
				pMsg->dwPChannel = dwPChannel;
				//pMsg->dwVirtualTrackID = smsg.m_dwVirtualTrackID;
				pMsg->dwType = DMUS_PMSGT_MIDI;

				// Initializes a few other DMUS_PMSG members
				hr = pDMGraph->StampPMsg((DMUS_PMSG*)pMsg);
								
				hr = m_pComponent->SendPMsg((DMUS_PMSG*)pMsg, m_pBand ? m_pBand->m_pAudiopath : NULL);
			}
		}
		break;
		case DM_PAN:
		{
			DMUS_MIDI_PMSG* pMsg = NULL;
			hr = m_pComponent->m_pIDMPerformance->AllocPMsg(sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG**)&pMsg);

			if(SUCCEEDED(hr))
			{
				ZeroMemory(pMsg, sizeof(DMUS_MIDI_PMSG));
				pMsg->dwSize = sizeof(DMUS_MIDI_PMSG);

				// DMUS_MIDI_PMSG members that need to be initialized 
				pMsg->bStatus = MIDI_CONTROL_CHANGE;
				pMsg->bByte1 = MIDI_CC_PAN;
				pMsg->bByte2 = bPan;
				// DMUS_PMSG members that need to be initialized
				pMsg->dwFlags |= DMUS_PMSGF_REFTIME;
				pMsg->dwPChannel = dwPChannel;
				//pMsg->dwVirtualTrackID = smsg.m_dwVirtualTrackID;
				pMsg->dwType = DMUS_PMSGT_MIDI;

				// Initializes a few other DMUS_PMSG members
				hr = pDMGraph->StampPMsg((DMUS_PMSG*)pMsg);
								
				hr = m_pComponent->SendPMsg((DMUS_PMSG*)pMsg, m_pBand ? m_pBand->m_pAudiopath : NULL);
			}
		}
		break;
		case DM_PITCHBEND_RANGE:
		{
			DMUS_CURVE_PMSG* pMsg = NULL;
			hr = m_pComponent->m_pIDMPerformance->AllocPMsg(sizeof(DMUS_PMSGT_CURVE), (DMUS_PMSG**)&pMsg);

			if(SUCCEEDED(hr))
			{
				ZeroMemory(pMsg, sizeof(DMUS_PMSGT_CURVE));
				pMsg->dwSize = sizeof(DMUS_PMSGT_CURVE);
				
				pMsg->dwFlags |= DMUS_PMSGF_DX8; // Pitch Bend is DX8 only flag
				pMsg->nEndValue = nPitchBendRange << 8;
				pMsg->bType = DMUS_CURVET_RPNCURVE;
				pMsg->bCurveShape = DMUS_CURVES_INSTANT;
				pMsg->wParamType = 0;

				// Initializes a few other DMUS_PMSG members
				hr = pDMGraph->StampPMsg((DMUS_PMSG*)pMsg);
								
				hr = m_pComponent->SendPMsg((DMUS_PMSG*)pMsg, m_pBand ? m_pBand->m_pAudiopath : NULL);
			}
		}
		break;

		default: //not supported
		{
			MessageBox(NULL, "Not supported mesasage", "Band Editor", MB_OK);
		}
	}
	pDMGraph->Release();
	return S_OK;
}


void CDMInstrument::SetDLSCollection( IDMUSProdReferenceNode* pDMReferenceNode )
{
	ASSERT( m_pBand != NULL );

	if( m_pDMReference != pDMReferenceNode )
	{
		if( m_pDMReference )
		{
			m_pBand->RemoveFromNotifyList( m_pDMReference ); 
			m_pDMReference->Release();
		}

		m_pDMReference = pDMReferenceNode;

		if( m_pDMReference != NULL )
		{
			m_pDMReference->AddRef();
			m_pBand->AddToNotifyList( m_pDMReference ); 
		}
	}
}


/////////////////////////////////////////////////////////////
// CInstrumentListItem Construction/Destruction

CInstrumentListItem::CInstrumentListItem(CDMInstrument* pInstrument) : m_dwRef(0)
{
	ASSERT(pInstrument);
	AddRef();

	m_pInstrument = pInstrument;
	m_fSelected = m_pInstrument->m_fSelected;
}

CInstrumentListItem::CInstrumentListItem() : m_fSelected(false)
{
	AddRef();
	m_pInstrument = NULL;
}


CInstrumentListItem::~CInstrumentListItem()
{
	if( m_pInstrument )
	{
		delete m_pInstrument;
		m_pInstrument = NULL;
	}
}

// IUnknown Implementation
HRESULT CInstrumentListItem::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(ppvObj);

	if(ppvObj == NULL)
	{
		return E_POINTER;
	}

	*ppvObj = NULL;

	if( IsEqualIID( riid, IID_IUnknown ) || IsEqualIID(riid, IID_IDMUSProdBandPChannel) )
	{
		AddRef();
		*ppvObj = (IDMUSProdBandPChannel*)this;
		return S_OK;
	}
	*ppvObj = NULL;
    return E_NOINTERFACE;
}


ULONG CInstrumentListItem::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CInstrumentListItem::Release()
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

// IDMUSProdBandPChannel Implementation
HRESULT CInstrumentListItem::GetPChannelNumber(int* pnNumber)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	 
	ASSERT(m_pInstrument);

	*pnNumber = m_pInstrument->dwPChannel;
	return S_OK; 
}

HRESULT CInstrumentListItem::GetVolume(int* pnVolume)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	 
	ASSERT(m_pInstrument);

	*pnVolume = (int)m_pInstrument->bVolume;
	return S_OK;
}

HRESULT CInstrumentListItem::GetPan(int* pnPan)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	 
	ASSERT(m_pInstrument);

	*pnPan = (int)m_pInstrument->bPan;
	return S_OK;
}

HRESULT CInstrumentListItem::SetVolume(int nVolume)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	 
	ASSERT(m_pInstrument);

	m_pInstrument->bVolume = (BYTE)nVolume;
	m_pInstrument->dwFlags |= DMUS_IO_INST_PATCH | DMUS_IO_INST_VOLUME | DMUS_IO_INST_PITCHBENDRANGE | DMUS_IO_INST_TRANSPOSE;
	return S_OK;
}

HRESULT CInstrumentListItem::SetPan(int nPan)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	 
	ASSERT(m_pInstrument);

	m_pInstrument->bPan = (BYTE) nPan;
	m_pInstrument->dwFlags |= DMUS_IO_INST_PATCH | DMUS_IO_INST_PAN | DMUS_IO_INST_PITCHBENDRANGE | DMUS_IO_INST_TRANSPOSE;
	return S_OK;
}

HRESULT CInstrumentListItem::IsSelected(BOOL* pfSelected)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	 

	*pfSelected = m_fSelected;
	return S_OK;
}

HRESULT CInstrumentListItem::SetSelected(BOOL fSelection)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	 

	m_fSelected = fSelection ? true : false;
	m_pInstrument->m_fSelected = m_fSelected;
	return S_OK;
}


HRESULT CInstrumentListItem::IsEnabled(BOOL* pbEnabled)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	 
	ASSERT(m_pInstrument);

	*pbEnabled = (m_pInstrument->dwFlags & DMUS_IO_INST_PATCH);
	return S_OK; 
}

HRESULT CInstrumentListItem::SyncChanges(BOOL bSendBandUpdate)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	 
	ASSERT(m_pInstrument);
	TRACE("In CInstrumentListItem::SyncChanges\n");

	CBand* pBand =  NULL;
	if(m_CommonPropertyObject.m_pBandDlg)
		pBand = m_CommonPropertyObject.m_pBandDlg->GetBand();
	
	// Send only the band update if this is true
	// This will be used to decide whether to send 
	// the other changes on mouse move on the grid
	if(bSendBandUpdate && pBand)
	{
		// Return failure if the update failed
		if(!m_pInstrument->SendBandUpdate(pBand))
			return E_FAIL;
		
		return S_OK;
	}

	m_pInstrument->m_bSendBandUpdate = false;
	if(pBand)
	{
		m_pInstrument->Send(pBand, DM_PAN);
		m_pInstrument->Send(pBand, DM_VOLUME);
	}

	return S_OK;
}


// Other methods
void CInstrumentListItem::SetSelected(bool bSelectionFlag)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	m_fSelected = bSelectionFlag;
	m_pInstrument->m_fSelected = m_fSelected;
}

void CInstrumentListItem::SetSelected()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	m_fSelected = !m_fSelected;
	m_pInstrument->m_fSelected = m_fSelected;
}

bool CInstrumentListItem::IsSelected()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return m_fSelected;
}

void CInstrumentListItem::SetInstrument(CDMInstrument* pInstrument)
{
	ASSERT(pInstrument);

	m_pInstrument = pInstrument;
}

CDMInstrument* CInstrumentListItem::GetInstrument()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return m_pInstrument;
}



////////////////////////////////////
// Common Properties Object

CCommonInstrumentPropertyObject::CCommonInstrumentPropertyObject():
m_dwChanged(0),
m_dwIgnoresChanged(0),
m_dwInstrumentChangeID(0),
m_nABIIndex(0),
m_dwRef(1), 
m_bReset(0),
m_pBandDlg(NULL),
m_nChannels(0),
m_nPChannelNumber(0),
m_sPChannelName(""),
m_dwPatch(0),
m_sPatch(""),
m_nOctave(0),
m_nTranspose(0),
m_nOctaveTranspose(0),
m_nVolume(0),
m_nPan(0),
m_dwPriority(0),
m_dwPriorityLevel(0),
m_dwPriorityOffset(0),
m_nLowNote(0),
m_nHighNote(0),
m_nSoundCanvas(0),
m_nPitchBendOctave(0),
m_nPitchBendTranspose(0),
m_nPitchBendRange(0),
m_nIgnoreVolume(0),
m_nIgnoreOctaveTranspose(0),
m_nIgnoreInstrument(0),
m_nIgnorePan(0),
m_nIgnorePriority(0),
m_nIgnoreNoteRange(0),
m_nIgnorePitchBendRange(0),
m_csCurrentCollection (""), 
m_nLastSelectedCollection(-1),
m_sInstrumentInfo(""),
m_sInstrumentCollectionInfo(""),
m_dwInstrumentPatchInfo(0),
m_bPreservePriorityOffset(false)
{

}

CCommonInstrumentPropertyObject::~CCommonInstrumentPropertyObject()
{
	// Remove PChannel from the property sheet
	IDMUSProdPropSheet* pIPropSheet;

	if( theApp.m_pIFramework && SUCCEEDED ( theApp.m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RemovePageManagerByObject( this );
		pIPropSheet->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCommonInstrumentPropertyObject IUnknown implementation

HRESULT CCommonInstrumentPropertyObject::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(ppvObj);
	if(ppvObj == NULL)
	{
		return E_POINTER;
	}

	*ppvObj = NULL;

	if( IsEqualIID( riid, IID_IUnknown ) || IsEqualIID(riid, IID_IDMUSProdPropPageObject) )
	{
		AddRef();
		*ppvObj = (IDMUSProdPropPageObject *)this;
		return S_OK;
	}
	*ppvObj = NULL;
    return E_NOINTERFACE;
}


ULONG CCommonInstrumentPropertyObject::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CCommonInstrumentPropertyObject::Release()
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
// CInstrumentListItem IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CInstrumentListItem IDMUSProdPropPageObject::GetData

HRESULT CCommonInstrumentPropertyObject::GetData( void** ppData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppData == NULL )
	{
		return E_POINTER;
	}

	*ppData = this;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdPropPageObject::SetData

HRESULT CCommonInstrumentPropertyObject::SetData( void* pData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pData);
	if(pData == NULL)
		return E_POINTER;

	CCommonInstrumentPropertyObject* pCommonObject = (CCommonInstrumentPropertyObject*)pData;
	ASSERT(pCommonObject);

	pCommonObject->ApplyChangesToSelectedChannelsAndRefreshUI();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdPropPageObject::OnShowProperties

HRESULT CCommonInstrumentPropertyObject::OnShowProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pIFramework != NULL );

	CDllBasePropPageManager* pPageManager;

	// Get the PChannel page manager
	if( theApp.m_pIPageManager
	&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_PChannelPropPageManager ) == S_OK )
	{
		pPageManager = (CDllBasePropPageManager *)theApp.m_pIPageManager;
	}
	else
	{
		pPageManager = new CPChannelPropPageManager();
	}
	if( pPageManager == NULL )
	{
		return E_FAIL;
	}

	// Save the focus so we can restore after changing the property page
	HWND hwndHadFocus;
	hwndHadFocus = ::GetFocus();

	// Show the PChannel properties
	IDMUSProdPropSheet* pIPropSheet = NULL;

	if( SUCCEEDED ( theApp.m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		if( SUCCEEDED ( pIPropSheet->SetPageManager(pPageManager) ) )
		{
			theApp.m_pIPageManager = pPageManager;
			pPageManager->SetObject( this );
		}

		pIPropSheet->Show( TRUE );
		pIPropSheet->Release();
	}

	// Restore the focus if it has changed
	if( hwndHadFocus != ::GetFocus() )
	{
		::SetFocus( hwndHadFocus );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBand IDMUSProdPropPageObject::OnRemoveFromPageManager

HRESULT CCommonInstrumentPropertyObject::OnRemoveFromPageManager( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}

void CCommonInstrumentPropertyObject::SetBandDialog(CBandDlg* pBandDlg)
{
	m_pBandDlg = pBandDlg;
}

CBandDlg* CCommonInstrumentPropertyObject::GetBandDialog()
{
	return m_pBandDlg;
}

void CCommonInstrumentPropertyObject::SetSelectedChannels(int nSelectedChannels)
{
	m_nChannels = nSelectedChannels;
}

int CCommonInstrumentPropertyObject::GetNumberOfChannels()
{
	return m_nChannels;
}

void CCommonInstrumentPropertyObject::SetReset(bool bReset)
{
	m_bReset = bReset;
}

void CCommonInstrumentPropertyObject::SetProperties(CBandDlg* pBandDlg, CDMInstrument* pInstrument)
{
	ASSERT(pBandDlg);
	ASSERT(pInstrument);

	m_pBandDlg = pBandDlg;
	
	SetPChannelNumber(pInstrument->dwPChannel);

	//Have to QI the Project for the PChannel Name : SetPChannelName();

	// Get the project that this band belongs to...
	CBand* pBand  = m_pBandDlg->GetBand();
	IDMUSProdProject* pIProject = NULL;
	IDMUSProdNode* pIDocRootNode = NULL;

	pBand->GetDocRootNode(&pIDocRootNode);

	pBand->m_pComponent->m_pIFramework->FindProject(pIDocRootNode, &pIProject);

	ASSERT(pIProject);
	
	IDMUSProdPChannelName* pIPChannelName;

	WCHAR pszPChannelName[DMUS_MAX_NAME];

	if(pIProject->QueryInterface(IID_IDMUSProdPChannelName, (void**)&pIPChannelName) == S_OK)
		pIPChannelName->GetPChannelName(pInstrument->dwPChannel, pszPChannelName);
	else
		wcscpy(pszPChannelName, (wchar_t*)"");
	

	if(pIPChannelName)
		pIPChannelName->Release();
	if(pIProject)
		pIProject->Release();
	if(pIDocRootNode)
		pIDocRootNode->Release();

	SetPChannelName(pszPChannelName);
	
	SetInstrument(pInstrument);

	 // Initialize m_nOctaveTranspose
	SetOctaveTranspose(pInstrument->nTranspose);
	SetOctave(pInstrument->nTranspose);
	SetTranspose(pInstrument->nTranspose);

	// Set the initial pitch bend range
	SetPitchBendRange(pInstrument->nPitchBendRange);
	SetPitchBendOctave(pInstrument->nPitchBendRange);
	SetPitchBendTranspose(pInstrument->nPitchBendRange);

	// Set inital volume & pan
	SetVolume(pInstrument->bVolume);
	SetPan(pInstrument->bPan);

	// Set initla note range
	SetLowNote(0);
	SetHighNote(127);
	
	WORD wNoteMaskIdx = 0, wNoteIdx = 0;
	DWORD dwNoteMask = pInstrument->dwNoteRanges[wNoteMaskIdx];

	while (wNoteIdx < 128 && wNoteMaskIdx < 4)
	{
		if (dwNoteMask & 1)
		{
			SetLowNote(wNoteIdx);
			break;
		}

		if (!(++wNoteIdx % 32))
		{
			dwNoteMask = pInstrument->dwNoteRanges[++wNoteMaskIdx];
		}
		else
		{
			dwNoteMask >>= 1;
		}
	}
	while (wNoteIdx < 128 && wNoteMaskIdx < 4)
	{
		if (!(dwNoteMask & 1))
		{
			ASSERT(GetLowNote() <= wNoteIdx - 1);
			SetHighNote(wNoteIdx - 1);
			break;
		}

		if (!(++wNoteIdx % 32))
		{
			dwNoteMask = pInstrument->dwNoteRanges[++wNoteMaskIdx];
		}
		else
		{
			dwNoteMask >>= 1;
		}
	}

	SetPriority(pInstrument->dwChannelPriority);			

	SetSoundCanvas(pInstrument);

	CheckAllIgnores(pInstrument);
}

void CCommonInstrumentPropertyObject::CheckAllIgnores(CDMInstrument* pInstrument)
{

	CheckInstrumentIgnore(pInstrument);

	CheckOctaveTransposeIgnore(pInstrument);

	CheckPitchBendIgnore(pInstrument);

	CheckVolumeIgnore(pInstrument);

	CheckPanIgnore(pInstrument);

	CheckPriorityIgnore(pInstrument);

	CheckNoteRangeIgnore(pInstrument);
}

void CCommonInstrumentPropertyObject::CheckInstrumentIgnore(CDMInstrument* pInstrument)
{	
	int nIgnoreFlag = IGNORE_OFF;

	if(!(pInstrument->dwFlags & DMUS_IO_INST_PATCH))
		nIgnoreFlag = IGNORE_ON;
	
	if(m_nChannels > 1 && m_nIgnoreInstrument != nIgnoreFlag && !m_bReset)
		nIgnoreFlag = IGNORE_UNDT;

	m_nIgnoreInstrument = nIgnoreFlag;
}

void CCommonInstrumentPropertyObject::CheckOctaveTransposeIgnore(CDMInstrument* pInstrument)
{
	int nIgnoreFlag = IGNORE_OFF;

	if(!(pInstrument->dwFlags & DMUS_IO_INST_TRANSPOSE))
		nIgnoreFlag = IGNORE_ON;
	
	if(m_nChannels > 1 && m_nIgnoreOctaveTranspose != nIgnoreFlag && !m_bReset)
		nIgnoreFlag = IGNORE_UNDT;

	m_nIgnoreOctaveTranspose = nIgnoreFlag;
}

void CCommonInstrumentPropertyObject::CheckPitchBendIgnore(CDMInstrument* pInstrument)
{
	int nIgnoreFlag = IGNORE_OFF;

	if(!(pInstrument->dwFlags & DMUS_IO_INST_PITCHBENDRANGE))
		nIgnoreFlag = IGNORE_ON;
	
	if(m_nChannels > 1 && m_nIgnorePitchBendRange != nIgnoreFlag && !m_bReset)
		nIgnoreFlag = IGNORE_UNDT;

	m_nIgnorePitchBendRange = nIgnoreFlag;
}


void CCommonInstrumentPropertyObject::CheckVolumeIgnore(CDMInstrument* pInstrument)
{
	int nIgnoreFlag = IGNORE_OFF;

	if(!(pInstrument->dwFlags & DMUS_IO_INST_VOLUME))
		nIgnoreFlag = IGNORE_ON;
	
	if(m_nChannels > 1 && m_nIgnoreVolume != nIgnoreFlag && !m_bReset)
		nIgnoreFlag = IGNORE_UNDT;

	m_nIgnoreVolume = nIgnoreFlag;
}

void CCommonInstrumentPropertyObject::CheckPanIgnore(CDMInstrument* pInstrument)
{
	int nIgnoreFlag = IGNORE_OFF;

	if(!(pInstrument->dwFlags & DMUS_IO_INST_PAN))
		nIgnoreFlag = IGNORE_ON;
	
	if(m_nChannels > 1 && m_nIgnorePan != nIgnoreFlag && !m_bReset)
		nIgnoreFlag = IGNORE_UNDT;

	m_nIgnorePan = nIgnoreFlag;
}

void CCommonInstrumentPropertyObject::CheckPriorityIgnore(CDMInstrument* pInstrument)
{
	int nIgnoreFlag = IGNORE_OFF;

	if(!(pInstrument->dwFlags & DMUS_IO_INST_CHANNEL_PRIORITY))
		nIgnoreFlag = IGNORE_ON;
	
	if(m_nChannels > 1 && m_nIgnorePriority != nIgnoreFlag && !m_bReset)
		m_nIgnorePriority = IGNORE_UNDT;
	
	m_nIgnorePriority = nIgnoreFlag;
}

void CCommonInstrumentPropertyObject::CheckNoteRangeIgnore(CDMInstrument* pInstrument)
{
	int nIgnoreFlag = IGNORE_OFF;

	if(!(pInstrument->dwFlags & DMUS_IO_INST_NOTERANGES))
		nIgnoreFlag = IGNORE_ON;
	
	if(m_nChannels > 1 && m_nIgnoreNoteRange != nIgnoreFlag && !m_bReset)
		nIgnoreFlag = IGNORE_UNDT;

	m_nIgnoreNoteRange = nIgnoreFlag;
}


void CCommonInstrumentPropertyObject::SetPChannelNumber(int nNumber)
{
	if(m_nChannels > 1)
		m_nPChannelNumber = -1;
	else
		m_nPChannelNumber = nNumber;
}

void CCommonInstrumentPropertyObject::SetPChannelName(CString sName)
{
	if(m_nChannels > 1)
		m_sPChannelName = achUnknown;
	else
		m_sPChannelName = sName;
}

void CCommonInstrumentPropertyObject::SetInstrument(CDMInstrument* pInstrument)
{
	ASSERT(pInstrument);
	if(pInstrument == NULL)
		return;

	int nPC = pInstrument->dwPatch & 0x7F;
	int nMSB = MSB(pInstrument->dwPatch);
	int nLSB = LSB(pInstrument->dwPatch);

	CBand* pBand = m_pBandDlg->GetBand();
	ASSERT(pBand);

	bool bIsGM = (0 != (pInstrument->dwFlags & DMUS_IO_INST_GM));
	bool bIsGS = (0 != (pInstrument->dwFlags & DMUS_IO_INST_GS));

	CString sCollection = pInstrument->m_csCollection;
	if(pInstrument->m_pDMReference == NULL && false == bIsGM && false == bIsGS)
		sCollection.LoadString(IDS_UNKNOWN);

	if(sCollection.IsEmpty() && bIsGM)
		sCollection.LoadString(IDS_GM);
	if(sCollection.IsEmpty() && bIsGS)
		sCollection.LoadString(IDS_GS);

	if(m_nChannels <= 1 || m_bReset)
	{
		m_sPatch = CBandDlg::GetPatchName(pBand->m_pComponent, pInstrument);
		m_dwInstrumentPatchInfo = pInstrument->dwPatch;
		m_sInstrumentCollectionInfo = sCollection;
		m_sInstrumentInfo.Format("%s (%d, %d, %d)", m_sInstrumentCollectionInfo, nMSB, nLSB, nPC);
	}
	else if(m_nChannels > 1)
	{
		CString sInstrumentInfo;
		sCollection = pInstrument->m_csCollection;
		if(sCollection.IsEmpty() && bIsGM)
			sCollection.LoadString(IDS_GM);
		else if(sCollection.IsEmpty() && bIsGS)
			sCollection.LoadString(IDS_GS);
		else
			sCollection.LoadString(IDS_UNKNOWN);

		if(m_sInstrumentCollectionInfo != sCollection)
			m_sInstrumentCollectionInfo = achUnknown;
		
		if(m_dwInstrumentPatchInfo != pInstrument->dwPatch)
		{
			m_dwInstrumentPatchInfo = 0xFFFFFFFF;
			sInstrumentInfo.Format("%s (%s)", m_sInstrumentCollectionInfo, achUnknown);
		}
		else
			sInstrumentInfo.Format("%s (%d, %d, %d)", m_sInstrumentCollectionInfo, nMSB, nLSB, nPC);

		m_sInstrumentInfo = sInstrumentInfo;

		if(m_sPatch != CBandDlg::GetPatchName(pBand->m_pComponent, pInstrument))
			m_sPatch = achUnknown;
	}
}

// Forced set on the instrument name while updating from the property page 
void CCommonInstrumentPropertyObject::SetInstrument(CString sInstrumentName)
{
	m_sPatch = sInstrumentName;
}

void CCommonInstrumentPropertyObject::SetPatch(DWORD dwPatch)
{
	m_dwPatch = dwPatch;
}

void CCommonInstrumentPropertyObject::SetOctave(short nTranspose)
{
	short nOctave = nTranspose/12;
	if(m_nChannels <= 1 || m_bReset)
	{
		m_nOctave = nOctave;
	}
	else if(m_nChannels > 1 && m_nOctaveTranspose != nTranspose)
		m_nOctave = -12;
}

void CCommonInstrumentPropertyObject::SetTranspose(short nTranspose)
{
	short nOctave = nTranspose / 12;
	
	short nFractionalTranspose = nTranspose - nOctave * 12;

	if(m_nChannels <= 1 || m_bReset)
	{
		m_nTranspose = nFractionalTranspose;
	}
	else if(m_nChannels > 1 && m_nOctaveTranspose != nTranspose)
		m_nTranspose = -12;
}


void CCommonInstrumentPropertyObject::SetOctaveTranspose(short nOctaveTranspose)
{
	if(m_nChannels <= 1 || m_bReset)
	{
		m_nOctave = (int)(nOctaveTranspose/12);
		m_nTranspose = nOctaveTranspose - m_nOctave * 12;
		m_nOctaveTranspose = nOctaveTranspose;
	}
	else if(m_nChannels > 1 && m_nOctaveTranspose != nOctaveTranspose)
		m_nTranspose = -12;
}


void CCommonInstrumentPropertyObject::SetPitchBendOctave(short nPitchBendRange)
{
	nPitchBendRange = nPitchBendRange > 127 ? 127 :  nPitchBendRange;
	nPitchBendRange = nPitchBendRange < 0 ? 0 :  nPitchBendRange;

	short nOctave = nPitchBendRange / 12;
	if(m_nChannels <= 1 || m_bReset)
	{
		m_nPitchBendOctave = nOctave;
	}
	else if(m_nChannels > 1 && m_nPitchBendRange != nPitchBendRange)
	{
		m_nPitchBendOctave = -12;
	}
}

void CCommonInstrumentPropertyObject::SetPitchBendTranspose(short nPitchBendRange)
{
	nPitchBendRange = nPitchBendRange > 127 ? 127 :  nPitchBendRange;
	nPitchBendRange = nPitchBendRange < 0 ? 0 :  nPitchBendRange;

	short nOctave = nPitchBendRange/12;
	
	short nFractionalTranspose = nPitchBendRange - nOctave * 12;

	if(m_nChannels <= 1 || m_bReset)
	{
		m_nPitchBendTranspose = nFractionalTranspose;
	}
	else if(m_nChannels > 1 && m_nPitchBendRange != nPitchBendRange)
		m_nPitchBendRange = -12;
}


void CCommonInstrumentPropertyObject::SetPitchBendRange(short nPitchBendRange)
{
	if(m_nChannels <= 1 || m_bReset)
	{
		nPitchBendRange = nPitchBendRange > 127 ? 127 :  nPitchBendRange;
		nPitchBendRange = nPitchBendRange < 0 ? 0 :  nPitchBendRange;

		m_nPitchBendOctave = (int)(nPitchBendRange / 12);
		m_nPitchBendTranspose = nPitchBendRange - m_nPitchBendOctave * 12;
		m_nPitchBendRange = nPitchBendRange;
	}
	else if(m_nChannels > 1 && m_nPitchBendRange!= nPitchBendRange)
	{
		m_nPitchBendRange = -12;
	}
}


void CCommonInstrumentPropertyObject::SetNoteRange(int nHighNote, int nLowNote)
{
	if(m_nChannels <= 1 || m_bReset)
	{
		nHighNote = nHighNote > 127 ? 127 :  nHighNote;
		nLowNote = nLowNote < 0 ? 0 :  nLowNote;

		m_nHighNote = short(nHighNote);
		m_nLowNote = short(nLowNote);
	}
	else if(m_nChannels > 1)
	{
		if(m_nLowNote != nLowNote)
		{
			m_nLowNote = -1;
		}
		if(m_nHighNote != nHighNote)
		{
			m_nHighNote = -1;
		}
	}
}

void CCommonInstrumentPropertyObject::SetVolume(int nVolume)
{
	if(m_nChannels <= 1 || m_bReset)
	{
		m_nVolume = nVolume;
	}
	else if(m_nChannels > 1 && m_nVolume != nVolume)
		m_nVolume = -1;
}
	

void CCommonInstrumentPropertyObject::SetPan(int nPan)
{
	if(m_nChannels <= 1 || m_bReset)
	{
		m_nPan = nPan;
	}
	else if(m_nChannels > 1 && m_nPan != nPan)
		m_nPan = -1;
}

// bool bPreserveOffset flag is used when the offset is not changed BUT 
// the priority level is changed on the property page 
void CCommonInstrumentPropertyObject::SetPriority(DWORD dwPriority, bool bPreserveOffset)
{
	m_bPreservePriorityOffset = bPreserveOffset;

	// Find out where this priority range falls
	int nCount = 0;
	while(dwPriority < dwaPriorityLevels[nCount] && nCount < PRIORITY_LEVELS)
		nCount++;
	
	DWORD dwPriorityLevel = dwaPriorityLevels[nCount];
	DWORD dwPriorityOffset = dwPriority - dwaPriorityLevels[nCount];

	if(m_nChannels <= 1 || m_bReset)
	{
		m_dwPriority = dwPriority;
		m_dwPriorityLevel = dwPriorityLevel;
		m_dwPriorityOffset = dwPriorityOffset;
	}
	else if(m_nChannels > 1 && (m_dwPriorityLevel != dwPriorityLevel) || (m_dwPriorityOffset != dwPriorityOffset))
	{
		if(m_dwPriorityLevel != dwPriorityLevel )
			m_dwPriorityLevel = 0xFFFFFFFF;

		if(m_dwPriorityOffset != dwPriorityOffset)
			m_dwPriorityOffset = 0xFFFFFFFF;
	}
	else
		m_dwPriority = m_dwPriority|dwPriority;
}

void CCommonInstrumentPropertyObject::SetLowNote(short nLowNote)
{
	ASSERT(nLowNote >= 0 && nLowNote <= 127);
	m_nLowNote = nLowNote;
}

void CCommonInstrumentPropertyObject::SetHighNote(short nHighNote)
{
	ASSERT(nHighNote >= 0 && nHighNote <= 127);
	m_nHighNote = nHighNote;
}


void CCommonInstrumentPropertyObject::SetSoundCanvas(CDMInstrument* pInstrument)
{
	int nState = 0;
	if(pInstrument->dwFlags & DMUS_IO_INST_USE_DEFAULT_GM_SET)
		nState = 1;
	
	if(m_nChannels <= 1 || m_bReset)
	{
		m_nSoundCanvas = nState;
	}
	else if(m_nChannels > 1 && m_nSoundCanvas != nState)
		m_nSoundCanvas = 2;
}

void CCommonInstrumentPropertyObject::SetSoundCanvas(int nState)
{
	m_nSoundCanvas = nState;
}


int  CCommonInstrumentPropertyObject::GetPChannelNumber()
{
	return m_nPChannelNumber;
}

CString CCommonInstrumentPropertyObject::GetPChannelName()
{
	return m_sPChannelName;
}

CString CCommonInstrumentPropertyObject::GetInstrument()
{
	return m_sPatch;
}

short CCommonInstrumentPropertyObject::GetOctave()
{
	return m_nOctave;
}

short CCommonInstrumentPropertyObject::GetTranspose()
{
	return m_nTranspose;
}

short CCommonInstrumentPropertyObject::GetOctaveTranspose()
{
	return m_nOctaveTranspose;
}


short CCommonInstrumentPropertyObject::GetPitchBendOctave()
{
	return m_nPitchBendOctave;
}

short CCommonInstrumentPropertyObject::GetPitchBendTranspose()
{
	return m_nPitchBendTranspose;
}

short CCommonInstrumentPropertyObject::GetPitchBendRange()
{
	return m_nPitchBendRange;
}

int CCommonInstrumentPropertyObject::GetVolume()
{
	return m_nVolume;
}

int CCommonInstrumentPropertyObject::GetPan()
{
	return m_nPan;
}

void CCommonInstrumentPropertyObject::GetPriority(DWORD& dwPriority, DWORD& dwOffset)
{
	// Don't do anything if it's indeterminate
	/*if(m_dwPriority == 0xFFFFFFFF)
		return;

		// Find out where this priority range falls
	int nCount = 0;
	while(m_dwPriority < dwaPriorityLevels[nCount] && nCount < PRIORITY_LEVELS)
		nCount++;
	
	dwPriority = dwaPriorityLevels[nCount];
	dwOffset = m_dwPriority - dwaPriorityLevels[nCount];*/

	dwPriority = m_dwPriorityLevel;
	dwOffset = m_dwPriorityOffset;
}

short CCommonInstrumentPropertyObject::GetLowNote()
{
	return m_nLowNote;
}

short CCommonInstrumentPropertyObject::GetHighNote()
{
	return m_nHighNote;
}

int CCommonInstrumentPropertyObject::GetSoundCanvas()
{
	return m_nSoundCanvas;
}


// All the SetIgnore methods....

void CCommonInstrumentPropertyObject::SetIgnoreInstrument(int nIgnore)
{
	m_nIgnoreInstrument = nIgnore;
}


void CCommonInstrumentPropertyObject::SetIgnoreOctaveTranspose(int nIgnore)
{
	m_nIgnoreOctaveTranspose = nIgnore;
}

void CCommonInstrumentPropertyObject::SetIgnorePitchBendRange(int nIgnore)
{
	m_nIgnorePitchBendRange = nIgnore;
}

void CCommonInstrumentPropertyObject::SetIgnoreVolume(int nIgnore)
{
	m_nIgnoreVolume = nIgnore;
}

void CCommonInstrumentPropertyObject::SetIgnorePan(int nIgnore)
{
	m_nIgnorePan = nIgnore;
}

void CCommonInstrumentPropertyObject::SetIgnorePriority(int nIgnore)
{
	m_nIgnorePriority = nIgnore;
}

void CCommonInstrumentPropertyObject::SetIgnoreNoteRange(int nIgnore)
{
	m_nIgnoreNoteRange = nIgnore;
}

void CCommonInstrumentPropertyObject::ApplyChangesToSelectedChannelsAndRefreshUI()
{
	ASSERT(m_pBandDlg);
	CBand* pBand = m_pBandDlg->GetBand();
	ASSERT(pBand);
	
	int nSelectedChannels = pBand->GetNumberOfSelectedInstruments();

	if(nSelectedChannels == 0)
		return;
	
	CInstrumentListItem** ppInstrumentsArray = new CInstrumentListItem* [nSelectedChannels];
	ASSERT(ppInstrumentsArray);

	pBand->GetSelectedInstruments(ppInstrumentsArray);

	for(int nCount=0; nCount < nSelectedChannels; nCount++)
	{
		CInstrumentListItem* pInstrumentItem = ppInstrumentsArray[nCount];
		ASSERT(pInstrumentItem);

		CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
		ASSERT(pInstrument);

		ApplyChanges(pInstrument);
	}

	// We need to update if somebody else does a Send
	CDMInstrument::m_bSendBandUpdate = TRUE;
	SyncPChannelChanges(nSelectedChannels, ppInstrumentsArray);
	CDMInstrument::m_bSendBandUpdate = TRUE;

	delete[] ppInstrumentsArray;	

	bool bResetChannelList = false;
	if(m_dwChanged & PCHANNEL_CHANGED)
	{
		bResetChannelList = true;
	}

	m_pBandDlg->UpdatePChannelChanges(bResetChannelList);
	
	// Reset all the temporary values;
	m_dwChanged = 0;
	m_dwInstrumentChangeID = 0;
	m_nABIIndex = 0;
}

void CCommonInstrumentPropertyObject::ApplyChanges(CDMInstrument* pInstrument)
{
	// Mark the band as modified so we get the "save" messagebox at exit time...
	m_pBandDlg->GetBand()->SetModifiedFlag( TRUE );

	if(m_dwChanged & OCTAVE_CHANGED)
		ApplyOctaveChange(pInstrument);
	
	if(m_dwChanged & PAN_CHANGED)
		ApplyPanChange(pInstrument);

	if(m_dwChanged & VOLUME_CHANGED)
		ApplyVolumeChange(pInstrument);

	if(m_dwChanged & INSTRUMENT_CHANGED)
		ApplyInstrumentChange(pInstrument);

	if(m_dwChanged & PCHANNEL_CHANGED)
		ApplyPChannelChange(pInstrument);

	if(m_dwChanged & NOTERANGE_CHANGED)
		ApplyNoteRangeChange(pInstrument);

	if(m_dwChanged & PITCHBEND_CHANGED)
		ApplyPitchBendRangeChange(pInstrument);

	if(m_dwChanged & PRIORITY_CHANGED)
		ApplyPriorityChange(pInstrument);

	if(m_dwChanged & SOUNDCANVAS_CHANGED)
		ApplySoundCanvasChange(pInstrument);

	if(m_dwChanged & IGNORES_CHANGED)
		ApplyIgnores(pInstrument);
}

void CCommonInstrumentPropertyObject::SyncPChannelChanges(int nChannels, CInstrumentListItem** ppInstrumentsArray)
{
	bool bDeleteInstrumentArray = false;
	if(ppInstrumentsArray == NULL)
	{
		ASSERT(m_pBandDlg);
		CBand* pBand = m_pBandDlg->GetBand();
		ASSERT(pBand);
		nChannels = pBand->GetNumberOfSelectedInstruments();

		if(nChannels == 0)
			return;
		
		ppInstrumentsArray = new CInstrumentListItem* [nChannels];
		ASSERT(ppInstrumentsArray);

		pBand->GetSelectedInstruments(ppInstrumentsArray);

		bDeleteInstrumentArray = true;
	}
	
	CDMInstrument::m_bSendBandUpdate = TRUE;
	BOOL bSendForBandUpdate = TRUE; 

	// Send the changes for each instrument
	for(int nCount = 0; nCount < nChannels; nCount++)
	{
		CInstrumentListItem* pInstrumentItem = ppInstrumentsArray[nCount];
		ASSERT(pInstrumentItem);

		CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
		ASSERT(pInstrument);

		// Do we need to send the band changes?
		if(bSendForBandUpdate)
		{
			bSendForBandUpdate = FALSE;
			if(!pInstrument->SendBandUpdate(m_pBandDlg->GetBand()))
				return;
		}

		CDMInstrument::m_bSendBandUpdate = FALSE;
		SyncChanges(pInstrument);
	}

	if(bDeleteInstrumentArray)
		delete[] ppInstrumentsArray;
}

void CCommonInstrumentPropertyObject::SyncChanges(CDMInstrument* pInstrument, CBand *pBand)
{
	ASSERT( pBand || (m_pBandDlg && m_pBandDlg->GetBand()) );

	if(m_dwChanged & OCTAVE_CHANGED)
		pInstrument->Send(pBand ? pBand : m_pBandDlg->GetBand(), DM_TRANSPOSE);
	
	if(m_dwChanged & PAN_CHANGED)
		pInstrument->Send(pBand ? pBand : m_pBandDlg->GetBand(), DM_PAN);

	if(m_dwChanged & VOLUME_CHANGED)
		pInstrument->Send(pBand ? pBand : m_pBandDlg->GetBand(), DM_VOLUME);

	if(m_dwChanged & INSTRUMENT_CHANGED)
		pInstrument->Send(pBand ? pBand : m_pBandDlg->GetBand(), DM_PATCH_CHANGE);

	if(m_dwChanged & PRIORITY_CHANGED)
		pInstrument->Send(pBand ? pBand : m_pBandDlg->GetBand(), DM_PATCH_CHANGE);

	if(m_dwChanged & SOUNDCANVAS_CHANGED)
		pInstrument->Send(pBand ? pBand : m_pBandDlg->GetBand(), DM_PATCH_CHANGE);

	if(m_dwChanged & IGNORES_CHANGED)
		SyncIgnores(pInstrument);
}



void CCommonInstrumentPropertyObject::SyncIgnores(CDMInstrument* pInstrument)
{
	if(m_dwChanged & IGNORE_INSTRUMENT_CHANGED)
		SyncInstrumentIgnore(pInstrument);

	if(m_dwChanged & IGNORE_OCTAVETRANSPOSE_CHANGED)
		SyncOctaveTransposeIgnore(pInstrument);

	if(m_dwChanged & IGNORE_VOLUME_CHANGED)
		SyncVolumeIgnore(pInstrument);

	if(m_dwChanged & IGNORE_PAN_CHANGED)
		SyncPanIgnore(pInstrument);

	if(m_dwChanged & IGNORE_PRIORITY_CHANGED)
		SyncPriorityIgnore(pInstrument);

	if(m_dwChanged & IGNORE_PITCHBEND_CHANGED)
		SyncPitchBendRangeIgnore(pInstrument);
}



void CCommonInstrumentPropertyObject::ApplyOctaveChange(CDMInstrument* pInstrument)
{
	pInstrument->nTranspose = GetOctaveTranspose();
}


void CCommonInstrumentPropertyObject::ApplyPitchBendRangeChange(CDMInstrument* pInstrument)
{
	pInstrument->nPitchBendRange = GetPitchBendRange();
}


void CCommonInstrumentPropertyObject::ApplyPanChange(CDMInstrument* pInstrument)
{
	pInstrument->bPan = BYTE(GetPan());
}


void CCommonInstrumentPropertyObject::ApplyVolumeChange(CDMInstrument* pInstrument)
{
	pInstrument->bVolume = BYTE(GetVolume());
}

void CCommonInstrumentPropertyObject::ApplyInstrumentChange(CDMInstrument* pInstrument, const CString *pcsCurrentCollection)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pInstrument);
	BInstr bi;

	if(m_dwInstrumentChangeID == DLS_REQUEST)
	{
		ASSERT( pcsCurrentCollection || (m_pBandDlg && m_pBandDlg->GetBand()) );

		CDlgDLS dlg;
		int nPChannel = pInstrument->dwPChannel;

		// initialize the data
		dlg.m_nLastSelectedCollection = m_nLastSelectedCollection;
		dlg.m_csCurrentCollection = m_csCurrentCollection;

		dlg.m_nTrack = nPChannel;
		dlg.m_nPC = pInstrument->dwPatch & 0x7F;
		dlg.m_nMSB = MSB(pInstrument->dwPatch);
		dlg.m_nLSB = LSB(pInstrument->dwPatch);
		
		if( pInstrument->m_csCollection.IsEmpty() == FALSE )
			dlg.m_csCurrentCollection = pInstrument->m_csCollection;
		else if(m_csCurrentCollection.IsEmpty())
		{
			if( pcsCurrentCollection )
			{
				dlg.m_csCurrentCollection = *pcsCurrentCollection;
			}
			else if( m_pBandDlg )
			{
				dlg.m_csCurrentCollection = m_pBandDlg->GetBand()->m_strDefaultCollection;
			}
		}
		
		// display the dialog
		if( IDOK == dlg.DoModal())
		{
			// set the new values
			m_csCurrentCollection = dlg.m_csCurrentCollection;
			m_nLastSelectedCollection = dlg.m_nLastSelectedCollection;
			pInstrument->m_csCollection = dlg.m_csCurrentCollection;
	
			pInstrument->dwPatch = MAKE_PATCH( (pInstrument->IsDrums()), dlg.m_nMSB, dlg.m_nLSB, dlg.m_nPC & 0x7F );

			pInstrument->dwFlags |= (DMUS_IO_INST_BANKSELECT);
			pInstrument->dwFlags &= ~(DMUS_IO_INST_GS | DMUS_IO_INST_GM);
			pInstrument->dwFlags |= 
				(DMUS_IO_INST_TRANSPOSE | DMUS_IO_INST_PATCH);

			// Store reference to DLS Collection
			pInstrument->SetDLSCollection( dlg.m_pICurrentRefNode );

			if( m_pBandDlg )
			{
				m_pBandDlg->GetBand()->SetModifiedFlag( TRUE );
			}
		}

		return;
	}

	// No Custom DLS
	pInstrument->m_csCollection = "";

	if(m_dwInstrumentChangeID == DRUMS_REQUEST) // If a drum request
		bi = abiDrums[m_nABIIndex];
	else										// A GM/GS request
		bi = abiGS[m_nABIIndex];


	int nPChannel = pInstrument->dwPChannel;
	
	pInstrument->dwPatch = MAKE_PATCH( (pInstrument->IsDrums()), bi.bMSB, bi.bLSB, bi.bPatch & 0x7F );

	// Clear GM/GS flags;
	pInstrument->dwFlags &= ~(DMUS_IO_INST_GS | DMUS_IO_INST_GM);
	if( bi.bMSB != 0	&&  pInstrument->IsDrums() == false)
	{
		pInstrument->dwFlags |= DMUS_IO_INST_BANKSELECT; 			// GS
		pInstrument->dwFlags |= DMUS_IO_INST_GS;
	}
	else
	{
		pInstrument->dwFlags &= ~DMUS_IO_INST_BANKSELECT;			// GM
		pInstrument->dwFlags |= DMUS_IO_INST_GM;
	}

	// Get rid of reference to DLS Collection
	pInstrument->SetDLSCollection( NULL );

	if( m_pBandDlg )
	{
		m_pBandDlg->GetBand()->SetModifiedFlag( TRUE );
	}
}

void CCommonInstrumentPropertyObject::ApplyPChannelChange(CDMInstrument* pInstrument)
{
	m_pBandDlg->ChangePChannelNumber(pInstrument, m_nPChannelNumber);
	m_nChannels = 0;
}

void CCommonInstrumentPropertyObject::ApplyNoteRangeChange(CDMInstrument* pInstrument)
{
	ASSERT(sizeof(pInstrument->dwNoteRanges) == 16);
	ZeroMemory(pInstrument->dwNoteRanges, 16);

	WORD wNoteIdx = (WORD)GetLowNote(), wNoteMax = (WORD)GetHighNote();
	WORD wNoteMaskIdx = wNoteIdx / 32;
	PDWORD pdwNoteMask = &pInstrument->dwNoteRanges[wNoteMaskIdx];

	while (wNoteIdx <= wNoteMax && wNoteMaskIdx < 4)
	{
		*pdwNoteMask |= 1 << (wNoteIdx % 32);
		if (!(++wNoteIdx % 32))
		{
			pdwNoteMask = &pInstrument->dwNoteRanges[++wNoteMaskIdx];
		}
	}
}

void CCommonInstrumentPropertyObject::ApplyPriorityChange(CDMInstrument* pInstrument)
{
	 // If we're changing only the level
	if(m_bPreservePriorityOffset)
	{
		// Find out where this priority range falls
		int nCount = 0;
		while(pInstrument->dwChannelPriority < dwaPriorityLevels[nCount] && nCount < PRIORITY_LEVELS)
			nCount++;
		
		DWORD dwOffset = pInstrument->dwChannelPriority - dwaPriorityLevels[nCount];
		pInstrument->dwChannelPriority = m_dwPriorityLevel|dwOffset;
	}
	else
		pInstrument->dwChannelPriority = m_dwPriority;
}

void CCommonInstrumentPropertyObject::ApplySoundCanvasChange(CDMInstrument* pInstrument)
{
	// We can't set the indeterminate state
	if(m_nSoundCanvas == 2)
		return;

	if(m_nSoundCanvas == 1)
		pInstrument->dwFlags |= DMUS_IO_INST_USE_DEFAULT_GM_SET;
	else
		pInstrument->dwFlags &= ~DMUS_IO_INST_USE_DEFAULT_GM_SET;
}


	
void CCommonInstrumentPropertyObject::ApplyIgnores(CDMInstrument* pInstrument)
{
	if(m_dwChanged & IGNORE_INSTRUMENT_CHANGED)
		ApplyInstrumentIgnore(pInstrument);

	if(m_dwChanged & IGNORE_OCTAVETRANSPOSE_CHANGED)
		ApplyOctaveTransposeIgnore(pInstrument);

	if(m_dwChanged & IGNORE_VOLUME_CHANGED)
		ApplyVolumeIgnore(pInstrument);

	if(m_dwChanged & IGNORE_PAN_CHANGED)
		ApplyPanIgnore(pInstrument);

	if(m_dwChanged & IGNORE_PRIORITY_CHANGED)
		ApplyPriorityIgnore(pInstrument);

	if(m_dwChanged & IGNORE_NOTERANGE_CHANGED)
		ApplyNoteRangeIgnore(pInstrument);

	if(m_dwChanged & IGNORE_PITCHBEND_CHANGED)
		ApplyPitchBendRangeIgnore(pInstrument);
}

void CCommonInstrumentPropertyObject::ApplyInstrumentIgnore(CDMInstrument* pInstrument)
{

	if(m_nIgnoreInstrument == IGNORE_ON)
	{
		pInstrument->dwFlags &= ~DMUS_IO_INST_PATCH;
		pInstrument->Send(m_pBandDlg->GetBand(), NULL);
	}
	else if(m_nIgnoreInstrument == IGNORE_OFF)
	{
		pInstrument->dwFlags |= DMUS_IO_INST_PATCH;
		pInstrument->Send(m_pBandDlg->GetBand(), DM_PATCH_CHANGE);
	}

	m_pBandDlg->GetBand()->SetModifiedFlag( TRUE );
}

void CCommonInstrumentPropertyObject::ApplyOctaveTransposeIgnore(CDMInstrument* pInstrument)
{
	DWORD temp = DMUS_IO_INST_TRANSPOSE;

	if(m_nIgnoreOctaveTranspose == IGNORE_ON)
	{
		pInstrument->dwFlags &= ~DMUS_IO_INST_TRANSPOSE; // Ignore the transpose
		pInstrument->Send(m_pBandDlg->GetBand(), NULL);
	}
	else if(m_nIgnoreOctaveTranspose == IGNORE_OFF)
	{
		pInstrument->dwFlags |= DMUS_IO_INST_TRANSPOSE;
		pInstrument->Send(m_pBandDlg->GetBand(), DM_TRANSPOSE);
	}

	m_pBandDlg->GetBand()->SetModifiedFlag( TRUE );
}


void CCommonInstrumentPropertyObject::ApplyPitchBendRangeIgnore(CDMInstrument* pInstrument)
{
	DWORD temp = DMUS_IO_INST_PITCHBENDRANGE;

	if(m_nIgnorePitchBendRange == IGNORE_ON)
	{
		pInstrument->dwFlags &= ~DMUS_IO_INST_PITCHBENDRANGE; // Ignore the transpose
		pInstrument->Send(m_pBandDlg->GetBand(), NULL);
	}
	else if(m_nIgnorePitchBendRange == IGNORE_OFF)
	{
		pInstrument->dwFlags |= DMUS_IO_INST_PITCHBENDRANGE;
		pInstrument->Send(m_pBandDlg->GetBand(), DM_PITCHBEND_RANGE);
	}

	m_pBandDlg->GetBand()->SetModifiedFlag( TRUE );
}


void CCommonInstrumentPropertyObject::ApplyVolumeIgnore(CDMInstrument* pInstrument)
{
	if(m_nIgnoreVolume == IGNORE_ON)
	{
		pInstrument->dwFlags &= ~DMUS_IO_INST_VOLUME; // Ignore the volume
		pInstrument->Send(m_pBandDlg->GetBand(), NULL);
	}
	else if(m_nIgnoreVolume == IGNORE_OFF)
	{
		pInstrument->dwFlags |= DMUS_IO_INST_VOLUME;
		pInstrument->Send(m_pBandDlg->GetBand(), DM_VOLUME);
	}

	m_pBandDlg->GetBand()->SetModifiedFlag( TRUE );
}

void CCommonInstrumentPropertyObject::ApplyPanIgnore(CDMInstrument* pInstrument)
{
	if(m_nIgnorePan == IGNORE_ON)
	{
		pInstrument->dwFlags &= ~DMUS_IO_INST_PAN; // Ignore the pan
		pInstrument->Send(m_pBandDlg->GetBand(), NULL);
	}
	else if(m_nIgnorePan == IGNORE_OFF)
	{
		pInstrument->dwFlags |= DMUS_IO_INST_PAN;
		pInstrument->Send(m_pBandDlg->GetBand(), DM_PAN);
	}
	m_pBandDlg->GetBand()->SetModifiedFlag( TRUE );
}

void CCommonInstrumentPropertyObject::ApplyPriorityIgnore(CDMInstrument* pInstrument)
{
	if(m_nIgnorePriority == IGNORE_ON)
	{
		pInstrument->dwFlags &= ~DMUS_IO_INST_CHANNEL_PRIORITY; // Ignore the pan
		pInstrument->Send(m_pBandDlg->GetBand(), NULL);
	}
	else if(m_nIgnorePriority == IGNORE_OFF)
	{
		pInstrument->dwFlags |= DMUS_IO_INST_CHANNEL_PRIORITY;
		pInstrument->Send(m_pBandDlg->GetBand(), DM_PATCH_CHANGE);
	}

	m_pBandDlg->GetBand()->SetModifiedFlag( TRUE );
}


void CCommonInstrumentPropertyObject::ApplyNoteRangeIgnore(CDMInstrument* pInstrument)
{
	if(m_nIgnoreNoteRange == IGNORE_ON)
		pInstrument->dwFlags &= ~DMUS_IO_INST_NOTERANGES; // Ignore the note range
	else if(m_nIgnoreNoteRange == IGNORE_OFF)
		pInstrument->dwFlags |= DMUS_IO_INST_NOTERANGES;
}


void CCommonInstrumentPropertyObject::SyncInstrumentIgnore(CDMInstrument* pInstrument)
{

	if(m_nIgnoreInstrument == IGNORE_ON)
	{
		pInstrument->Send(m_pBandDlg->GetBand(), NULL);
	}
	else if(m_nIgnoreInstrument == IGNORE_OFF)
	{
		pInstrument->Send(m_pBandDlg->GetBand(), DM_PATCH_CHANGE);
	}
}


void CCommonInstrumentPropertyObject::SyncOctaveTransposeIgnore(CDMInstrument* pInstrument)
{
	if(m_nIgnoreOctaveTranspose == IGNORE_ON)
	{
		pInstrument->Send(m_pBandDlg->GetBand(), NULL);
	}
	else if(m_nIgnoreOctaveTranspose == IGNORE_OFF)
	{
		pInstrument->Send(m_pBandDlg->GetBand(), DM_TRANSPOSE);
	}
}


void CCommonInstrumentPropertyObject::SyncPitchBendRangeIgnore(CDMInstrument* pInstrument)
{
	if(m_nIgnorePitchBendRange == IGNORE_ON)
	{
		pInstrument->Send(m_pBandDlg->GetBand(), NULL);
	}
	else if(m_nIgnorePitchBendRange == IGNORE_OFF)
	{
		pInstrument->Send(m_pBandDlg->GetBand(), DM_PITCHBEND_RANGE);
	}
}


void CCommonInstrumentPropertyObject::SyncVolumeIgnore(CDMInstrument* pInstrument)
{
	if(m_nIgnoreVolume == IGNORE_ON)
	{
		pInstrument->Send(m_pBandDlg->GetBand(), NULL);
	}
	else if(m_nIgnoreVolume == IGNORE_OFF)
	{
		pInstrument->Send(m_pBandDlg->GetBand(), DM_VOLUME);
	}
}

void CCommonInstrumentPropertyObject::SyncPanIgnore(CDMInstrument* pInstrument)
{
	if(m_nIgnorePan == IGNORE_ON)
	{
		pInstrument->Send(m_pBandDlg->GetBand(), NULL);
	}
	else if(m_nIgnorePan == IGNORE_OFF)
	{
		pInstrument->Send(m_pBandDlg->GetBand(), DM_PAN);
	}
}

void CCommonInstrumentPropertyObject::SyncPriorityIgnore(CDMInstrument* pInstrument)
{
	if(m_nIgnorePriority == IGNORE_ON)
	{
		pInstrument->Send(m_pBandDlg->GetBand(), NULL);
	}
	else if(m_nIgnorePriority == IGNORE_OFF)
	{
		pInstrument->Send(m_pBandDlg->GetBand(), DM_PATCH_CHANGE);
	}
}
