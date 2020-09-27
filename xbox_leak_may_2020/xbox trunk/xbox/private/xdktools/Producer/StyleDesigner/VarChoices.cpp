// CVarChoices.cpp : implementation file
//

#include "stdafx.h"
#include "VarChoices.h"
#include "Timeline.h"
#include "StyleDesignerDLL.h"
#include "StyleComponent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// {EE59D340-88D9-11d1-89AF-00A0C9054129}
static const GUID GUID_VarChoicesPropPageManager = 
{ 0xee59d340, 0x88d9, 0x11d1, { 0x89, 0xaf, 0x0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };


//////////////////////////////////////////////////////////////////////
// CVarChoicesPropPageManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVarChoicesPropPageManager::CVarChoicesPropPageManager()
{
	m_pTabVarChoices = NULL;
	m_GUIDManager = GUID_VarChoicesPropPageManager;
}

CVarChoicesPropPageManager::~CVarChoicesPropPageManager()
{
	if( m_pTabVarChoices )
	{
		delete m_pTabVarChoices;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesPropPageManager::RemoveCurrentObject

void CVarChoicesPropPageManager::RemoveCurrentObject( void )
{
	if( m_pIPropPageObject == NULL )
	{
		return;
	}

	m_pIPropPageObject->OnRemoveFromPageManager();
	m_pIPropPageObject = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesPropPageManager IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CVarChoicesPropPageManager::GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strVarChoices;

	strVarChoices.LoadString( IDS_VARIATIONS_TEXT );

	CString strTitle = strVarChoices;

	CVarChoices* pVarChoices;

	if( m_pIPropPageObject
	&& (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pVarChoices))) )
	{
		strTitle = pVarChoices->m_strTitle;
		/*
		strTitle.Empty();

		// "Style name - "
		if( pVarChoices->m_pPattern
		&&  pVarChoices->m_pPattern->m_pStyle )
		{
			strTitle = pVarChoices->m_pPattern->m_pStyle->m_strName;
			strTitle += _T(" - " );
		}

		// "Pattern name "
		if( pVarChoices->m_pPattern )
		{
			strTitle += pVarChoices->m_pPattern->m_strName;
			strTitle += _T(" " );
		}

		// "(Track name)"
		if( pVarChoices->m_pPartRef )
		{
			CString strTrack;
			TCHAR achTemp[MID_BUFFER];

			strTrack.LoadString( IDS_TRACK );
			sprintf( achTemp, "%s %d", strTrack, pVarChoices->m_pPartRef->m_dwPChannel + 1 );
			strTrack = achTemp;

			strTitle += _T("(" );
			strTitle += strTrack;
			strTitle += _T(") " );
		}
		*/

		// "Variation Choices"
		strTitle += _T(" ") + strVarChoices;
	}

	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesPropPageManager IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CVarChoicesPropPageManager::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages )
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

	// Add VarChoices tab
	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	m_pTabVarChoices = new CTabVarChoices( this );
	if( m_pTabVarChoices )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabVarChoices->m_psp );
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
// CVarChoicesPropPageManager IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CVarChoicesPropPageManager::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDllBasePropPageManager::OnRemoveFromPropertySheet();

	theApp.m_pIPageManager = NULL;

	Release();	// delete myself

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesPropPageManager IDMUSProdPropPageManager::RefreshData

HRESULT CVarChoicesPropPageManager::RefreshData( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CVarChoices* pVarChoices;
	
	if( m_pIPropPageObject == NULL )
	{
		pVarChoices = NULL;
	}
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pVarChoices ) ) )
	{
		return E_FAIL;
	}

	// Make sure changes to current VarChoices are processed in OnKillFocus
	// messages before setting the new VarChoices
	CWnd* pWndHadFocus = CWnd::GetFocus();
	CWnd* pWnd = pWndHadFocus;
	CWnd* pWndParent = m_pTabVarChoices->GetParent();

	while( pWnd )
	{
		if( pWnd == pWndParent )
		{
			::SetFocus( NULL );
			break;
		}
		pWnd = pWnd->GetParent();
	}

	// Set Property tabs to display the new VarChoices
	m_pTabVarChoices->SetVarChoices( pVarChoices );

	// Restore focus
	if( pWndHadFocus
	&&  pWndHadFocus != CWnd::GetFocus() )
	{
		pWndHadFocus->SetFocus();
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CVarChoices constructor/destructor

CVarChoices::CVarChoices()
{
    m_dwRef = 0;
	AddRef();

	/*
	if( m_pPattern->m_pStyle )
	{
		m_pIDocRootNode = m_pPattern->m_pStyle;
	}
	else
	*/
	{
		m_pIDocRootNode = NULL;
	}

	m_pIParentNode = NULL;

	m_hWndEditor = NULL;
	m_pVarChoicesCtrl = NULL;
	m_pCallback = NULL;
	m_nUndoText = 0;
	m_fInPaste = false;

	memset( &m_wp, 0, sizeof(WINDOWPLACEMENT) );
	memset( &m_dwSelectedFlagBtns, 0, sizeof(m_dwSelectedFlagBtns) );
	memset( &m_bSelectedRowBtns, 0, sizeof(m_bSelectedRowBtns) );

	for( int i = 0 ;  i < NBR_VARIATIONS ;  i++ )
	{
		m_dwVariationChoices[i] = (DM_VF_MODE_DMUSIC | DM_VF_FLAG_BITS);	
	}
}

CVarChoices::~CVarChoices()
{
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	// Remove CVarChoices from clipboard
	theApp.FlushClipboard( this );

	// Remove CVarChoices from property sheet
	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RemovePageManagerByObject( this );
		RELEASE( pIPropSheet );
	}

	// Release the callback pointer
	RELEASE( m_pCallback );
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IUnknown implementation

HRESULT CVarChoices::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdNode)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        *ppvObj = (IDMUSProdNode *)this;
    }
    else if( ::IsEqualIID(riid, IID_IPersist) )
    {
        *ppvObj = (IPersist *)this;
    }
    else if( ::IsEqualIID(riid, IID_IPersistStream) )
    {
        *ppvObj = (IPersistStream *)this;
    }
	else if( ::IsEqualIID(riid, IID_IVarChoices) )
	{
		*ppvObj = (IVarChoices *)this;
	}
	else if( ::IsEqualIID(riid, IID_IDMUSProdPropPageObject) )
	{
		*ppvObj = (IDMUSProdPropPageObject *)this;
	}
	else
	{
		*ppvObj = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

ULONG CVarChoices::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CVarChoices::Release()
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
// CVarChoices IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::GetNodeImageIndex

HRESULT CVarChoices::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );

	return( theApp.m_pStyleComponent->GetPatternImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::GetFirstChild

HRESULT CVarChoices::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(ppIFirstChildNode);

    return E_NOTIMPL;	// VarChoices node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::GetNextChild

HRESULT CVarChoices::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(ppINextChildNode);

    return E_NOTIMPL;	// VarChoices node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::GetComponent

HRESULT CVarChoices::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );

	return theApp.m_pStyleComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::GetDocRootNode

HRESULT CVarChoices::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CVarChoices IDMUSProdNode::SetDocRootNode

HRESULT CVarChoices::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::GetParentNode

HRESULT CVarChoices::GetParentNode( IDMUSProdNode** ppIParentNode )
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
// CVarChoices IDMUSProdNode::SetParentNode

HRESULT CVarChoices::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::GetNodeId

HRESULT CVarChoices::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_VarChoicesNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::GetNodeName

HRESULT CVarChoices::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;
	strName.LoadString( IDS_NONE_TEXT );
	*pbstrName = strName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::GetNodeNameMaxLength

HRESULT CVarChoices::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = -1;	// Can't rename a VarChoices node

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::ValidateNodeName

HRESULT CVarChoices::ValidateNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a VarChoices node
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::SetNodeName

HRESULT CVarChoices::SetNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a VarChoices node
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::GetNodeListInfo

HRESULT CVarChoices::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
//	AFX_MANAGE_STATE( _afxModuleAddrThis );
	UNREFERENCED_PARAMETER(pListInfo);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::GetEditorClsId

HRESULT CVarChoices::GetEditorClsId( CLSID* pClsId )
{
 	AFX_MANAGE_STATE(_afxModuleAddrThis);

   *pClsId = CLSID_VarChoicesEditor;
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::GetEditorTitle

HRESULT CVarChoices::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strTitle;
	TCHAR achTitle[MID_BUFFER];

	// "Variations: "
	::LoadString( theApp.m_hInstance, IDS_VARIATIONS_TEXT, achTitle, MID_BUFFER );
	strTitle  = achTitle;
	strTitle += _T(": " );

	strTitle += m_strTitle;

	/*
	// "Style name - "
	if( m_pPattern
	&&  m_pPattern->m_pStyle )
	{
		strTitle += m_pPattern->m_pStyle->m_strName;
		strTitle += _T(" - " );
	}

	// "Pattern name "
	if( m_pPattern )
	{
		strTitle += m_pPattern->m_strName;
		strTitle += _T(" " );
	}

	// "(Track name)"
	if( m_pPartRef )
	{
		CString strTrack;
		TCHAR achTemp[MID_BUFFER];

		strTrack.LoadString( IDS_TRACK );
		sprintf( achTemp, "%s %d", strTrack, m_pPartRef->m_dwPChannel + 1 );
		strTrack = achTemp;

		strTitle += _T("(" );
		strTitle += strTrack;
		strTitle += _T(")" );
	}
	*/

    *pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::GetEditorWindow

HRESULT CVarChoices::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*hWndEditor = m_hWndEditor;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::SetEditorWindow

HRESULT CVarChoices::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_hWndEditor = hWndEditor;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::UseOpenCloseImages

HRESULT CVarChoices::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::GetRightClickMenuId

HRESULT CVarChoices::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(phInstance);
	UNREFERENCED_PARAMETER(pnMenuId);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::OnRightClickMenuInit

HRESULT CVarChoices::OnRightClickMenuInit( HMENU hMenu )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(hMenu);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::OnRightClickMenuSelect

HRESULT CVarChoices::OnRightClickMenuSelect( long lCommandId )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(lCommandId);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::DeleteChildNode

HRESULT CVarChoices::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_NOTIMPL;	// VarChoices nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::InsertChildNode

HRESULT CVarChoices::InsertChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;	// VarChoices nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::DeleteNode

HRESULT CVarChoices::DeleteNode( BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fPromptUser);

	// Remove from Component Style list
	ASSERT( 0 );
	// Remove from pattern or part ref or part list
	// Or set part's single varchoices class to null

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::OnNodeSelChanged

HRESULT CVarChoices::OnNodeSelChanged( BOOL fSelected )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fSelected);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::CreateDataObject

HRESULT CVarChoices::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

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

	// Save Style into stream
	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		if( SUCCEEDED ( Save( pIStream, FALSE ) ) )
		{
			// Place CF_VARCHOICES into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( theApp.m_pStyleComponent->m_cfVarChoices, pIStream ) ) )
			{
				hr = S_OK;
			}
		}

		RELEASE( pIStream );
	}

	if( SUCCEEDED ( hr ) )
	{
		*ppIDataObject = pDataObject;	// already AddRef'd
	}
	else
	{
		RELEASE( pDataObject );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::CanCut

HRESULT CVarChoices::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return S_FALSE;		// Can't remove a VarChoices node
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::CanCopy

HRESULT CVarChoices::CanCopy( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	for( short nRow = 0 ;  nRow < NBR_VARIATIONS ;  nRow++ )
	{
		if( m_bSelectedRowBtns[nRow] )
		{
			return S_OK;
		}
	}
 
	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::CanDelete

HRESULT CVarChoices::CanDelete( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return S_FALSE;		// Can't remove a VarChoices node
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::CanDeleteChildNode

HRESULT CVarChoices::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	ASSERT( 0 );

	return E_NOTIMPL;	// VarChoices nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::CanPasteFromData

HRESULT CVarChoices::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
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

	if( m_pVarChoicesCtrl == NULL
	||  m_pVarChoicesCtrl->m_pVarChoicesDlg == NULL )
	{
		return E_UNEXPECTED;
	}

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = S_FALSE;
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfVarChoices ) ) )
	{
		hr = S_OK;
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::PasteFromData

HRESULT CVarChoices::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	if( m_pVarChoicesCtrl == NULL
	||  m_pVarChoicesCtrl->m_pVarChoicesDlg == NULL )
	{
		return E_UNEXPECTED;
	}

	// Create a new CDllJazzDataObject to get the data object's stream.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = E_FAIL;

	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfVarChoices ) ) )
	{
		// Handle CF_VARCHOICES format
		hr = PasteCF_VARCHOICES( pDataObject, pIDataObject );
		if( SUCCEEDED ( hr ) )
		{
			m_pVarChoicesCtrl->m_pVarChoicesDlg->m_fDirty = true;
			m_pVarChoicesCtrl->m_pVarChoicesDlg->SyncPattern();
		}
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::CanChildPasteFromData

HRESULT CVarChoices::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
											BOOL* pfWillSetReference )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(pfWillSetReference);

	ASSERT( 0 );

	return E_NOTIMPL;	// VarChoices nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::ChildPasteFromData

HRESULT CVarChoices::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);

	ASSERT( 0 );

	return E_NOTIMPL;	// VarChoices nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdNode::GetObject

HRESULT CVarChoices::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(rclsid);
	UNREFERENCED_PARAMETER(riid);
	UNREFERENCED_PARAMETER(ppvObject);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdPropPageObject::GetData

HRESULT CVarChoices::GetData( void** ppData )
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
// CVarChoices IDMUSProdPropPageObject::SetData

HRESULT CVarChoices::SetData( void* pData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pData);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdPropPageObject::OnShowProperties

HRESULT CVarChoices::OnShowProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	// Get the VarChoices page manager
	CVarChoicesPropPageManager* pPageManager;

	if( theApp.m_pIPageManager
	&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_VarChoicesPropPageManager ) == S_OK )
	{
		pPageManager = (CVarChoicesPropPageManager *)theApp.m_pIPageManager;
	}
	else
	{
		pPageManager = new CVarChoicesPropPageManager();
	}

	if( pPageManager == NULL )
	{
		return E_FAIL;
	}

	// Show the VarChoices properties
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		if( SUCCEEDED ( pIPropSheet->SetPageManager(pPageManager) ) )
		{
			theApp.m_pIPageManager = pPageManager;
			pPageManager->SetObject( this );
		}

		pIPropSheet->Show( TRUE );
		RELEASE( pIPropSheet );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IDMUSProdPropPageObject::OnRemoveFromPageManager

HRESULT CVarChoices::OnRemoveFromPageManager( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CVarChoices IPersist::GetClassID

HRESULT CVarChoices::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( pClsId != NULL );

    memset( pClsId, 0, sizeof( CLSID ) );

    return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
// CVarChoices IPersistStream::IsDirty

HRESULT CVarChoices::IsDirty()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IPersistStream::Load

HRESULT CVarChoices::Load( IStream* pIStream )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( pIStream != NULL );
	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;
    HRESULT hr = E_FAIL;

	if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		ckMain.fccType = DM_FOURCC_VARCHOICES_FORM;

		if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
		{
			hr = LoadVarChoices( pIRiffStream, &ckMain );

			if( m_fInPaste == false )
			{
				// Sync change with property sheet
				IDMUSProdPropSheet* pIPropSheet;
				if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
				{
					pIPropSheet->RefreshTitleByObject( this );
					pIPropSheet->RefreshActivePageByObject( this );
					RELEASE( pIPropSheet );
				}

				// Sync changes with the dialog
				if( m_pVarChoicesCtrl
				&&	m_pVarChoicesCtrl->m_pVarChoicesDlg )
				{
					m_pVarChoicesCtrl->m_pVarChoicesDlg->RefreshControls();
				}
			}
		}

		RELEASE( pIRiffStream );
	}

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IPersistStream::Save

HRESULT CVarChoices::Save( IStream* pIStream, BOOL fClearDirty )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fClearDirty);

	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	DMUSProdStreamInfo StreamInfo;
	StreamInfo.ftFileType = FT_RUNTIME;
	StreamInfo.guidDataFormat = GUID_CurrentVersion;
	StreamInfo.pITargetDirectoryNode = NULL;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		RELEASE( pPersistInfo );
	}

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;

    HRESULT hr = E_FAIL;

	if( IsEqualGUID( StreamInfo.guidDataFormat, GUID_CurrentVersion ) )
	{
		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			ckMain.fccType = DM_FOURCC_VARCHOICES_FORM;

			if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATERIFF ) == 0
			&&  SUCCEEDED( SaveVarChoices( pIRiffStream ) )
			&&  pIRiffStream->Ascend( &ckMain, 0 ) == 0 )
			{
				hr = S_OK;
			}
			RELEASE( pIRiffStream );
		}
	}

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IPersistStream::GetSizeMax

HRESULT CVarChoices::GetSizeMax( ULARGE_INTEGER FAR* /*pcbSize*/ )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices Additional functions

/////////////////////////////////////////////////////////////////////////////
// CVarChoices::PasteCF_VARCHOICES

HRESULT CVarChoices::PasteCF_VARCHOICES( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject )
{
	DWORD dwCurVariationChoices[NBR_VARIATIONS];
	DWORD dwCurSelectedFlagBtns[NBR_VARIATIONS];
	BYTE bCurSelectedRowBtns[NBR_VARIATIONS];	

	DWORD dwClipVariationChoices[NBR_VARIATIONS];
	DWORD dwClipSelectedFlagBtns[NBR_VARIATIONS];
	BYTE bClipSelectedRowBtns[NBR_VARIATIONS];	

	IStream* pIStream;
	HRESULT hr = E_FAIL;

	// Save current flags
	memcpy( &dwCurVariationChoices, &m_dwVariationChoices, sizeof(dwCurVariationChoices) );
	memcpy( &dwCurSelectedFlagBtns, &m_dwSelectedFlagBtns, sizeof(dwCurSelectedFlagBtns) );
	memcpy( &bCurSelectedRowBtns, &m_bSelectedRowBtns, sizeof(bCurSelectedRowBtns) );

	if( SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, theApp.m_pStyleComponent->m_cfVarChoices, &pIStream  ) ) )
	{
		LARGE_INTEGER liTemp;

		// Seek to beginning of stream
		liTemp.QuadPart = 0;
		pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

		m_fInPaste = true;
		hr = Load( pIStream );
		m_fInPaste = false;

		if( SUCCEEDED ( hr ) )
		{
			// Save clipboard flags
			memcpy( &dwClipVariationChoices, &m_dwVariationChoices, sizeof(dwClipVariationChoices) );
			memcpy( &dwClipSelectedFlagBtns, &m_dwSelectedFlagBtns, sizeof(dwClipSelectedFlagBtns) );
			memcpy( &bClipSelectedRowBtns, &m_bSelectedRowBtns, sizeof(bClipSelectedRowBtns) );

			// Restore current flags
			memcpy( &m_dwVariationChoices, &dwCurVariationChoices, sizeof(m_dwVariationChoices) );
			memcpy( &m_dwSelectedFlagBtns, &dwCurSelectedFlagBtns, sizeof(m_dwSelectedFlagBtns) );
			memcpy( &m_bSelectedRowBtns, &bCurSelectedRowBtns, sizeof(m_bSelectedRowBtns) );

			short nNbrCurRows = 0;
			short nNbrClipRows = 0;

			for( short nRow = 0 ;  nRow < NBR_VARIATIONS ;  nRow++ )
			{
				if( bClipSelectedRowBtns[nRow] )
				{
					nNbrClipRows++;
				}

				if( bCurSelectedRowBtns[nRow] )
				{
					nNbrCurRows++;
				}
			}

			// Paste takes place here!!!
			if( nNbrClipRows == nNbrCurRows )
			{
				short nClipRow = 0;
				
				m_nUndoText = IDS_UNDO_PATTERN_VARCHOICES_PASTE;

				for( short nCurRow = 0 ; nCurRow < NBR_VARIATIONS ; nCurRow++ )
				{
					if( bCurSelectedRowBtns[nCurRow] )
					{
						for( ;  nClipRow < NBR_VARIATIONS ;  nClipRow++ )
						{
							if( bClipSelectedRowBtns[nClipRow] )
							{
								m_dwVariationChoices[nCurRow] = dwClipVariationChoices[nClipRow];
								nClipRow++;
								break;
							}
							ASSERT( nClipRow != (NBR_VARIATIONS - 1) );
						}
					}
				}
			}
			else
			{
				HINSTANCE hInstance = AfxGetResourceHandle();
				AfxSetResourceHandle( theApp.m_hInstance );
				AfxMessageBox( IDS_ERR_VARCHOICES_PASTE );
				AfxSetResourceHandle( hInstance );
				hr = E_FAIL;
			}
		}

		RELEASE( pIStream );
	}

	if( SUCCEEDED ( hr ) )
	{
		memset( &m_dwSelectedFlagBtns, 0, sizeof(m_dwSelectedFlagBtns) );
		memset( &m_bSelectedRowBtns, 0, sizeof(m_bSelectedRowBtns) );
	}
	else
	{
		// Restore current flags
		memcpy( &m_dwVariationChoices, &dwCurVariationChoices, sizeof(m_dwVariationChoices) );
		memcpy( &m_dwSelectedFlagBtns, &dwCurSelectedFlagBtns, sizeof(m_dwSelectedFlagBtns) );
		memcpy( &m_bSelectedRowBtns, &bCurSelectedRowBtns, sizeof(m_bSelectedRowBtns) );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices::SaveVarChoices

HRESULT CVarChoices::SaveVarChoices( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	int i;
	ioVarChoices oVarChoices;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write VarChoices chunk header
	ck.ckid = DM_FOURCC_VARCHOICES_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare ioVarChoices structure
	memset( &oVarChoices, 0, sizeof(ioVarChoices) );

	for( i = 0 ;  i < NBR_VARIATIONS ;  i++ )
	{
		oVarChoices.m_dwVariationChoices[i] = m_dwVariationChoices[i];
		oVarChoices.m_dwSelectedFlagBtns[i] = m_dwSelectedFlagBtns[i];
		oVarChoices.m_bSelectedRowBtns[i] = m_bSelectedRowBtns[i];	
	}

	// Write VarChoices chunk data
	hr = pIStream->Write( &oVarChoices, sizeof(ioVarChoices), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(ioVarChoices) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
	}
	
	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices::LoadVarChoices

HRESULT CVarChoices::LoadVarChoices( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
    IStream* pIStream;
	MMCKINFO ck;
	DWORD dwByteCount;
	DWORD dwSize;
	int	i;
    HRESULT hr = S_OK;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case DM_FOURCC_VARCHOICES_CHUNK:
			{
				ioVarChoices iVarChoices;

				dwSize = min( ck.cksize, sizeof( ioVarChoices ) );
				hr = pIStream->Read( &iVarChoices, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				for( i = 0 ;  i < NBR_VARIATIONS ;  i++ )
				{
					m_dwVariationChoices[i] = iVarChoices.m_dwVariationChoices[i];
					m_dwSelectedFlagBtns[i] = iVarChoices.m_dwSelectedFlagBtns[i];
					m_bSelectedRowBtns[i] = iVarChoices.m_bSelectedRowBtns[i];	
				}
				break;
			}
        }

        pIRiffStream->Ascend( &ck, 0 );
    }

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IVarChoices implementation

/////////////////////////////////////////////////////////////////////////////
// CVarChoices IVarChoices::SetVarChoicesTitle

HRESULT CVarChoices::SetVarChoicesTitle(BSTR bstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( bstrTitle == NULL )
	{
		return E_POINTER;
	}

	m_strTitle = bstrTitle;

	::SysFreeString( bstrTitle );

	// TODO: Update name of open windows?

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IVarChoices::SetDataChangedCallback

HRESULT CVarChoices::SetDataChangedCallback( IUnknown *punkCallback )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( punkCallback == NULL )
	{
		return E_POINTER;
	}

	RELEASE( m_pCallback );

	return punkCallback->QueryInterface( IID_IDMUSProdTimelineCallback, (void **)&m_pCallback );
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoices IVarChoices::GetUndoText

HRESULT CVarChoices::GetUndoText( BSTR *pbstrUndoText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pbstrUndoText == NULL )
	{
		return E_POINTER;
	}

	CString strUndoText;
	if( strUndoText.LoadString( m_nUndoText ) )
	{
		*pbstrUndoText = strUndoText.AllocSysString();
		return S_OK;
	}

	return E_FAIL;
}
