// PropertySheetCtrl.cpp : implementation file
//

/*-----------
@doc DMUSPROD
-----------*/

#include "stdafx.h"
#include "JazzApp.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "PropertySheetCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define WM_REFRESH_TITLE	(WM_USER + 0xF0)


/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl

IMPLEMENT_DYNAMIC(CPropertySheetCtrl, CPropertySheet)

CPropertySheetCtrl::CPropertySheetCtrl() : CPropertySheet( _T(""), NULL, 0 )
{
	m_dwRef = 1;

	m_pIPageManager = NULL;
	m_pDummyPage = NULL;
	m_fDummyPageInserted = FALSE;
	m_nNbrPages = 0;
	m_nActivePage = -1;
}

CPropertySheetCtrl::~CPropertySheetCtrl()
{
	// Pages were removed from property sheet in CMainframe::OnDestroyWindow

	if( m_pIPageManager )
	{
		m_pIPageManager->Release();
	}

	if( m_pDummyPage )
	{
		delete m_pDummyPage;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl::InsertDummyPage

void CPropertySheetCtrl::InsertDummyPage()
{
	if( m_fDummyPageInserted == FALSE )
	{
		if( m_pDummyPage == NULL )
		{
			m_pDummyPage = new CPropertyPage( IDD_TAB_DUMMY );
		}

		if( m_pDummyPage )
		{
			AddPage( m_pDummyPage );
			m_fDummyPageInserted = TRUE;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl::RemoveDummyPage

void CPropertySheetCtrl::RemoveDummyPage()
{
	if( m_fDummyPageInserted == TRUE )
	{
		if( m_pDummyPage )
		{
			RemovePage( m_pDummyPage );
			m_fDummyPageInserted = FALSE;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl::InsertThePage

BOOL CPropertySheetCtrl::InsertThePage( HPROPSHEETPAGE hPage )
{
	if( hPage == NULL )
	{
		return FALSE;
	}

	// Insert page internally
	m_lstPages.AddTail( (HPROPSHEETPAGE)hPage );

	// Insert page externally
	SendMessage( PSM_ADDPAGE, 0, (LPARAM)hPage );
	m_nNbrPages++;

	RemoveDummyPage();

	// Activate first page 
	if( m_lstPages.GetCount() == 1 )
	{
		SendMessage( PSM_SETCURSEL, 0, NULL );
	}

	// Show tab control
	ShowWindow( SW_SHOW );

	// Hide control with "No Properties available" text
	CWnd* pWnd = GetParent()->GetDlgItem( IDC_NOTABS );
	if( pWnd )
	{
		pWnd->ShowWindow( SW_HIDE );
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl::ChangeTitle

void CPropertySheetCtrl::ChangeTitle( BSTR bstrTitle, BOOL fAddPropertiesText )
{
	CString strTitle = bstrTitle;
	::SysFreeString( bstrTitle );

	CWnd* pWnd1 = GetParent();
	if( pWnd1 )
	{
		pWnd1 = pWnd1->GetParent();
		if( pWnd1 )
		{
			CWnd* pWnd2 = pWnd1->GetParent();
			if( pWnd2 )
			{
				CString strNewTitle;
				CString strOldTitle;

				// Determine new title
				HINSTANCE hInstance = AfxGetResourceHandle();
				AfxSetResourceHandle( theApp.m_hInstance );

				if( strTitle.IsEmpty() )
				{
					strNewTitle.LoadString( IDS_PROPERTIES );
				}
				else if( fAddPropertiesText )
				{
					AfxFormatString1( strNewTitle, IDS_PLUS_PROPERTIES, strTitle );
				}
				else
				{
					strNewTitle = strTitle;
				}

				AfxSetResourceHandle( hInstance );

				// Get old title
				pWnd2->GetWindowText( strOldTitle );

				if( strNewTitle != strOldTitle )
				{
					pWnd1->SetWindowText( strNewTitle );
					pWnd2->SetWindowText( strNewTitle );
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl::RemoveCurrentPageManager

void CPropertySheetCtrl::RemoveCurrentPageManager( void )
{
	if( m_pIPageManager == NULL )
	{
		return;
	}

	// Get the active page before we start causing it
	//	to change because of processing in this function
	// m_nActivePage must be reset to -1 before leaving
	//	this function
	CTabCtrl* pTab = GetTabControl();
	ASSERT_VALID(pTab);
	m_nActivePage = short(pTab->GetCurSel());
	
	// Change title of Properties window
	CString strTitle;
	TCHAR achTitle[MID_BUFFER];

	::LoadString( theApp.m_hInstance, IDS_PROPERTIES, achTitle, MID_BUFFER );
	strTitle = achTitle;
	BSTR bstrTitle = strTitle.AllocSysString();
	ChangeTitle( bstrTitle, FALSE );

	// Temporarily hide controls to avoid flashing
	ShowWindow( SW_HIDE );
	CWnd* pWndNoTabs = GetParent()->GetDlgItem( IDC_NOTABS );
	if( pWndNoTabs )
	{
		pWndNoTabs->ShowWindow( SW_HIDE );
	}

	InsertDummyPage();

	// Remove all pages from property sheet (except active page)
	HPROPSHEETPAGE hActivePage = NULL;
	HPROPSHEETPAGE hPage;

	int nCount = 0;
	while( !m_lstPages.IsEmpty() )
	{
		hPage = static_cast<HPROPSHEETPAGE>( m_lstPages.RemoveHead() );
		if( nCount == m_nActivePage )
		{
			hActivePage = hPage;
		}
		else
		{
			SendMessage( PSM_REMOVEPAGE, 0, (LPARAM)hPage );
		}
		nCount++;
	}

	// Now remove the active page
	if( hActivePage )
	{
		SendMessage( PSM_REMOVEPAGE, 0, (LPARAM)hActivePage );
	}

	// Reset page count
	m_nNbrPages = 0;

	// Show with "No Properties available" text
	if( pWndNoTabs )
	{
		pWndNoTabs->ShowWindow( SW_SHOW );
	}

	m_pIPageManager->OnRemoveFromPropertySheet();
	m_pIPageManager->Release();
	m_pIPageManager = NULL;
	m_nActivePage = -1;

	// Make sure property sheet does NOT have the focus
	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	// Get the active MDI child window.
	CChildFrame *pMDIChild = (CChildFrame *)pMainFrame->GetActiveFrame();
	if( pMDIChild )
	{
		pMDIChild->SetFocusToEditor();
	}
	else
	{
		pMainFrame->SetFocus();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl IDMUSProdPropSheet implementation

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl::QueryInterface

HRESULT CPropertySheetCtrl::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    if( ::IsEqualIID(riid, IID_IDMUSProdFramework) )
    {
		if( theApp.m_pFramework )
		{
			return theApp.m_pFramework->QueryInterface( riid, ppvObj );
		}
		else
		{
		    *ppvObj = NULL;
		    return E_NOINTERFACE;
		}
    }

    if( ::IsEqualIID(riid, IID_IDMUSProdPropSheet)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDMUSProdPropSheet *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CPropertySheetCtrl::AddRef()
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return ++m_dwRef;
}

ULONG CPropertySheetCtrl::Release()
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    ASSERT( m_dwRef != 0 );

    --m_dwRef;

    if( m_dwRef == 0 )
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}


#define MAX_PROPSHEET_PAGES	50

/*======================================================================================
METHOD:  IDMUSPRODPROPSHEET::SETPAGEMANAGER
========================================================================================
@method HRESULT | IDMUSProdPropSheet | SetPageManager | Sets the global property sheet's
	<o PropPageManager> to <p pINewPageManager>. 
 
@comm
	<i IDMUSProdPropSheet> provides the means for a <o Component> to hook into the Framework's
	property sheet control, or <o PropSheet> object.  The Framework creates the PropSheet
	object during application launch and it exists throughout the entire DirectMusic Producer
	work session. A pointer to the Framework's IDMUSProdPropSheet interface can be obtained via
	<om IDMUSProdFramework::QueryInterface>.

	This method simply returns S_OK when the page manager is already set to <p pINewPageManager>.

@rvalue S_OK | The property sheet's page manager was successfully set to <p pINewPageManager>.
@rvalue E_INVALIDARG | <p pINewPageManager> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and the page manager could not be set.
@rvalue E_UNEXPECTED | This method was called recursively.  This method may only be called once at a time.

@ex The following implementation of <om IDMUSProdPropPageObject.OnShowProperties> displays
	properties for a Pattern in a Style: |

HRESULT CDirectMusicPattern::OnShowProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDllBasePropPageManager* pPageManager;
	short nActiveTab = 0;

	// Get the Pattern page manager
	if( theApp.m_pIPageManager
	&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_PatternPropPageManager ) == S_OK )
	{
		pPageManager = (CDllBasePropPageManager *)theApp.m_pIPageManager;
	}
	else
	{
		pPageManager = new CPatternPropPageManager();
	}

	if( pPageManager == NULL )
	{
		return E_FAIL;
	}

	// Show the Pattern properties
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		if( SUCCEEDED ( pIPropSheet->SetPageManager(pPageManager) ) )
		{
			theApp.m_pIPageManager = pPageManager;
			pPageManager->SetObject( this );
			pIPropSheet->SetActivePage( nActiveTab ); 
		}

		pIPropSheet->Show( TRUE );
		pIPropSheet->Release();
	}

	return S_OK;
}
	
@xref <i IDMUSProdPropSheet>, <om IDMUSProdPropSheet.RemovePageManager>, <om IDMUSProdPropSheet.RemovePageManagerByObject>, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl  IDMUSProdPropSheet::SetPageManager

HRESULT FAR EXPORT CPropertySheetCtrl::SetPageManager(
	IDMUSProdPropPageManager* pINewPageManager	// @parm [in] Pointer to the <i IDMUSProdPropPageManager>
												//			interface to be attached to the property sheet
												//			control.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pINewPageManager == NULL )
	{
		return E_INVALIDARG;
	}

	if( pINewPageManager == m_pIPageManager )
	{
		return S_OK;
	}

	static bool fEnteredFunction = false;

	if( fEnteredFunction )
	{
		return E_UNEXPECTED;
	}

	fEnteredFunction = true;

	RemoveCurrentPageManager();

	m_pIPageManager = pINewPageManager;
	m_pIPageManager->AddRef();

	LONG* hPropSheetPage[MAX_PROPSHEET_PAGES];
	short nNbrPages;
	
	if( SUCCEEDED ( m_pIPageManager->GetPropertySheetPages( this, hPropSheetPage, &nNbrPages ) ) )
	{
		for( int i=0 ;  i<nNbrPages ;  i++ )
		{
			InsertThePage( (HPROPSHEETPAGE)hPropSheetPage[i] );
		}

		RefreshTitle();
	}

	fEnteredFunction = false;

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODPROPSHEET::REMOVEPAGEMANAGER
========================================================================================
@method HRESULT | IDMUSProdPropSheet | RemovePageManager | Removes <p pIPageManager> from
	the <o Framework>'s global property sheet. 
 
@comm
	<i IDMUSProdPropSheet> provides the means for a <o Component> to hook into the Framework's
	property sheet control, or <o PropSheet> object.  The Framework creates the PropSheet
	object during application launch and it exists throughout the entire DirectMusic Producer
	work session. A pointer to the Framework's IDMUSProdPropSheet interface can be obtained via
	<om IDMUSProdFramework::QueryInterface>.

@rvalue S_OK | <p pIPageManager> was removed successfully.
@rvalue E_INVALIDARG | <p pIPageManager> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and <p pIPageManager> was not removed.

@ex The following code excerpt removes <p pIPageManager> from the Framework's property sheet: |

	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED( pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void **) &pIPropSheet ) ) )
	{
		pIPropSheet->RemovePageManager( pIPageManager );
		pIPropSheet->Release();
	}
	
@xref <i IDMUSProdPropSheet>, <om IDMUSProdPropSheet.SetPageManager>, <om IDMUSProdPropSheet.RemovePageManagerByObject>, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl  IDMUSProdPropSheet::RemovePageManager

HRESULT FAR EXPORT CPropertySheetCtrl::RemovePageManager(
	IDMUSProdPropPageManager* pIPageManager		// @parm [in] Pointer to the <i IDMUSProdPropPageManager>
												//			interface to be removed.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( (pIPageManager == NULL)
	||  (pIPageManager != m_pIPageManager) )
	{
		return E_INVALIDARG;
	}

	RemoveCurrentPageManager();

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODPROPSHEET::REMOVEPAGEMANAGERBYOBJECT
========================================================================================
@method HRESULT | IDMUSProdPropSheet | RemovePageManagerByObject | Removes the current 
	<o PropPageManager> from the <o Framework>'s global property sheet when properties
	for <p pIPropPageObject> are being displayed. 
 
@comm
	<i IDMUSProdPropSheet> provides the means for a <o Component> to hook into the Framework's
	property sheet control, or <o PropSheet> object.  The Framework creates the PropSheet
	object during application launch and it exists throughout the entire DirectMusic Producer
	work session. A pointer to the Framework's IDMUSProdPropSheet interface can be obtained via
	<om IDMUSProdFramework::QueryInterface>.

	This method simply returns S_OK when <p pIPropPageObject> is not being displayed in the
	property sheet.

@rvalue S_OK | The current page manager was removed successfully.
@rvalue E_INVALIDARG | <p pIPropPageObject> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and the current page manager was not removed.

@ex The following example shows an easy way of ensuring the property sheet does not display
	objects that have been destructed: |

CFileNode::~CFileNode()
{
	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED( pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void **) &pIPropSheet ) ) )
	{
		pIPropSheet->RemovePageManagerByObject( this );
		pIPropSheet->Release();
	}

	...
	...
	...
}
	
@xref <i IDMUSProdPropSheet>, <om IDMUSProdPropSheet.SetPageManager>, <om IDMUSProdPropSheet.RemovePageManager>, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl  IDMUSProdPropSheet::RemovePageManagerByObject

HRESULT FAR EXPORT CPropertySheetCtrl::RemovePageManagerByObject(
	IDMUSProdPropPageObject* pIPropPageObject	// @parm [in] Pointer to an <i IDMUSProdPropPageObject> interface.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pIPropPageObject == NULL )
	{
		return E_INVALIDARG;
	}

	if( IsEqualPageManagerObject( pIPropPageObject ) == S_OK )
	{
		RemoveCurrentPageManager();
	}

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODPROPSHEET::REFRESHTITLEBYOBJECT
========================================================================================
@method HRESULT | IDMUSProdPropSheet | RefreshTitleByObject | Redraws the property sheet
	title when properties for <p pIPropPageObject> are being displayed.
 
@comm
	<i IDMUSProdPropSheet> provides the means for a <o Component> to hook into the Framework's
	property sheet control, or <o PropSheet> object.  The Framework creates the PropSheet
	object during application launch and it exists throughout the entire DirectMusic Producer
	work session. A pointer to the Framework's IDMUSProdPropSheet interface can be obtained via
	<om IDMUSProdFramework::QueryInterface>.

@rvalue S_OK | The property sheet title was refreshed successfully.
@rvalue E_INVALIDARG | <p pIPropPageObject> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and the property sheet title was not refreshed.

@ex The following code excerpt refreshes properties after an object changes: |

	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED( pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void **) &pIPropSheet ) ) )
	{
		pIPropSheet->RefreshTitleByObject( this );
		pIPropSheet->RefreshActivePageByObject( this );
		pIPropSheet->Release();
	}
	
@xref <i IDMUSProdPropSheet>, <om IDMUSProdPropSheet.RefreshActivePageByObject>, <om IDMUSProdPropSheet.RefreshActivePage>,
		<om IDMUSProdPropSheet.RefreshTitle>, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl  IDMUSProdPropSheet::RefreshTitleByObject

HRESULT FAR EXPORT CPropertySheetCtrl::RefreshTitleByObject(
	IDMUSProdPropPageObject* pIPropPageObject	// @parm [in] Pointer to an <i IDMUSProdPropPageObject> interface.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pIPropPageObject == NULL )
	{
		return E_INVALIDARG;
	}

	if( IsEqualPageManagerObject( pIPropPageObject ) == S_OK )
	{
		m_pIPageManager->RefreshData();
		RefreshTitle();
	}

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODPROPSHEET::REFRESHACTIVEPAGEBYOBJECT
========================================================================================
@method HRESULT | IDMUSProdPropSheet | RefreshActivePageByObject | Redraws the active
	page when properties for <p pIPropPageObject> are being displayed. 
 
@comm
	<i IDMUSProdPropSheet> provides the means for a <o Component> to hook into the Framework's
	property sheet control, or <o PropSheet> object.  The Framework creates the PropSheet
	object during application launch and it exists throughout the entire DirectMusic Producer
	work session. A pointer to the Framework's IDMUSProdPropSheet interface can be obtained via
	<om IDMUSProdFramework::QueryInterface>.

@rvalue S_OK | The active page was refreshed successfully.
@rvalue E_INVALIDARG | <p pIPropPageObject> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and the active page was not refreshed.

@ex The following code excerpt refreshes properties after an object changes: |

	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED( pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void **) &pIPropSheet ) ) )
	{
		pIPropSheet->RefreshTitleByObject( this );
		pIPropSheet->RefreshActivePageByObject( this );
		pIPropSheet->Release();
	}
	
@xref <i IDMUSProdPropSheet>, <om IDMUSProdPropSheet.RefreshTitleByObject>, <om IDMUSProdPropSheet.RefreshTitle>,
		<om IDMUSProdPropSheet.RefreshActivePage>, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl  IDMUSProdPropSheet::RefreshActivePageByObject

HRESULT FAR EXPORT CPropertySheetCtrl::RefreshActivePageByObject(
	IDMUSProdPropPageObject* pIPropPageObject	// @parm [in] Pointer to an <i IDMUSProdPropPageObject> interface.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pIPropPageObject == NULL )
	{
		return E_INVALIDARG;
	}

	if( IsEqualPageManagerObject( pIPropPageObject ) == S_OK )
	{
		m_pIPageManager->RefreshData();
		RefreshActivePage();
	}

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODPROPSHEET::ISEQUALPAGEMANAGEROBJECT
========================================================================================
@method HRESULT | IDMUSProdPropSheet | IsEqualPageManagerObject | Determines whether 
	<p pIPropPageObject> is currently being displayed in the <o Framework>'s global
	property sheet.
 
@comm
	<i IDMUSProdPropSheet> provides the means for a <o Component> to hook into the Framework's
	property sheet control, or <o PropSheet> object.  The Framework creates the PropSheet
	object during application launch and it exists throughout the entire DirectMusic Producer
	work session. A pointer to the Framework's IDMUSProdPropSheet interface can be obtained via
	<om IDMUSProdFramework::QueryInterface>.

@rvalue S_OK | <p pIPropPageObject> is being displayed in the property sheet.
@rvalue S_FALSE | <p pIPropPageObject> is not being displayed in the propertysheet.
@rvalue E_INVALIDARG | <p pIPropPageObject> is not valid.  For example, it may be NULL.

@ex The following code excerpt initiates display of properties every time a different <o Node>
	object is clicked in the Project Tree: |

	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED ( pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		if( pIPropSheet->IsShowing() == S_OK )
		{
			// Get properties for node
			IDMUSProdPropPageObject* pIPageObject;
			if( SUCCEEDED ( pINode->QueryInterface( IID_IDMUSProdPropPageObject, (void **)&pIPageObject ) ) )
			{
				if( pIPropSheet->IsEqualPageManagerObject(pIPageObject) != S_OK )
				{
					pIPageObject->OnShowProperties();
				}
				pIPageObject->Release();
			}
		}

		pIPropSheet->Release();
	}
	
@xref <i IDMUSProdPropSheet>, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl  IDMUSProdPropSheet::IsEqualPageManagerObject

HRESULT FAR EXPORT CPropertySheetCtrl::IsEqualPageManagerObject(
	IDMUSProdPropPageObject* pIPropPageObject	// @parm [in] Pointer to an <i IDMUSProdPropPageObject> interface.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pIPropPageObject == NULL )
	{
		return E_INVALIDARG;
	}

	if( m_pIPageManager )
	{
		return m_pIPageManager->IsEqualObject( pIPropPageObject );
	}

	return S_FALSE;
}


/*======================================================================================
METHOD:  IDMUSPRODPROPSHEET::REFRESHTITLE
========================================================================================
@method HRESULT | IDMUSProdPropSheet | RefreshTitle | Redraws the title of the <o Framework>'s
	global property sheet.
 
@comm
	<i IDMUSProdPropSheet> provides the means for a <o Component> to hook into the Framework's
	property sheet control, or <o PropSheet> object.  The Framework creates the PropSheet
	object during application launch and it exists throughout the entire DirectMusic Producer
	work session. A pointer to the Framework's IDMUSProdPropSheet interface can be obtained via
	<om IDMUSProdFramework::QueryInterface>.

    Only use this method when you are sure that the object being displayed in the property sheet
	needs to be refreshed.  When an object changes and you do not know whether or not its properties
	are being displayed use the <om IDMUSProdPropSheet.RefreshTitleByObject> method.

@rvalue S_OK | Always succeeds.
	
@xref <i IDMUSProdPropSheet>, <om IDMUSProdPropSheet.RefreshTitleByObject>, <om IDMUSProdPropSheet.RefreshActivePageByObject>,
		<om IDMUSProdPropSheet.RefreshActivePage>, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl  IDMUSProdPropSheet::RefreshTitle

HRESULT FAR EXPORT CPropertySheetCtrl::RefreshTitle( void )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	PostMessage( WM_REFRESH_TITLE, 0, 0L );

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODPROPSHEET::REFRESHACTIVEPAGE
========================================================================================
@method HRESULT | IDMUSProdPropSheet | RefreshActivePage | Redraws the active page of the
	<o Framework>'s global property sheet. 
 
@comm
	<i IDMUSProdPropSheet> provides the means for a <o Component> to hook into the Framework's
	property sheet control, or <o PropSheet> object.  The Framework creates the PropSheet
	object during application launch and it exists throughout the entire DirectMusic Producer
	work session. A pointer to the Framework's IDMUSProdPropSheet interface can be obtained via
	<om IDMUSProdFramework::QueryInterface>.

    Only use this method when you are sure that the object being displayed in the property sheet
	needs to be refreshed.  When an object changes and you do not know whether or not its properties
	are being displayed use the <om IDMUSProdPropSheet.RefreshActivePageByObject> method.

@rvalue S_OK | Always succeeds.
	
@xref <i IDMUSProdPropSheet>, <om IDMUSProdPropSheet.RefreshActivePageByObject>, <om IDMUSProdPropSheet.RefreshTitleByObject>,
		<om IDMUSProdPropSheet.RefreshTitle>, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl  IDMUSProdPropSheet::RefreshActivePage

HRESULT FAR EXPORT CPropertySheetCtrl::RefreshActivePage( void )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CTabCtrl* pTab = GetTabControl();
	ASSERT_VALID(pTab);

	int nIndex = pTab->GetCurSel();
	if( nIndex >= 0 )
	{
		HWND hwndFocus = ::GetFocus();
		SendMessage( PSM_SETCURSEL, nIndex, NULL );
		if( ::IsWindow( hwndFocus )
		&&	hwndFocus != ::GetFocus() )
		{
			::SetFocus( hwndFocus );
		}
	}

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODPROPSHEET::GETACTIVEPAGE
========================================================================================
@method HRESULT | IDMUSProdPropSheet | GetActivePage | Returns the active page of the
	<o Framework>'s global property sheet. 
 
@comm
	<i IDMUSProdPropSheet> provides the means for a <o Component> to hook into the Framework's
	property sheet control, or <o PropSheet> object.  The Framework creates the PropSheet
	object during application launch and it exists throughout the entire DirectMusic Producer
	work session. A pointer to the Framework's IDMUSProdPropSheet interface can be obtained via
	<om IDMUSProdFramework::QueryInterface>.

@rvalue S_OK | The active page was returned in <p pnIndex>.
@rvalue E_POINTER | <p pnIndex> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and the active page was not returned.
	
@xref <i IDMUSProdPropSheet>, <om IDMUSProdPropSheet.SetActivePage>, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl  IDMUSProdPropSheet::GetActivePage

HRESULT FAR EXPORT CPropertySheetCtrl::GetActivePage(
	short* pnIndex		// @parm [out,retval] Pointer to the caller-allocated variable
						//		in which the active page is to be returned.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pnIndex == NULL )
	{
		return E_POINTER;
	}

	*pnIndex = -1;

	CTabCtrl* pTab = GetTabControl();
	ASSERT_VALID(pTab);

	if( m_nActivePage == -1 )
	{
		*pnIndex = short(pTab->GetCurSel());
	}
	else
	{
		*pnIndex = m_nActivePage;
	}

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODPROPSHEET::SETACTIVEPAGE
========================================================================================
@method HRESULT | IDMUSProdPropSheet | SetActivePage | Sets the active page of the
	<o Framework>'s global property sheet to <p nIndex>. 
 
@comm
	<i IDMUSProdPropSheet> provides the means for a <o Component> to hook into the Framework's
	property sheet control, or <o PropSheet> object.  The Framework creates the PropSheet
	object during application launch and it exists throughout the entire DirectMusic Producer
	work session. A pointer to the Framework's IDMUSProdPropSheet interface can be obtained via
	<om IDMUSProdFramework::QueryInterface>.

@rvalue S_OK | The active page was set to <p nIndex>.
@rvalue E_INVALIDARG | <p nIndex> is not valid.  For example, it may be less than zero.
@rvalue E_FAIL | An error occurred, and the active page was not set.

@ex The following implementation of <om IDMUSProdPropPageObject.OnShowProperties> displays
	properties for a Pattern in a Style: |

HRESULT CDirectMusicPattern::OnShowProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDllBasePropPageManager* pPageManager;
	short nActiveTab = 0;

	// Get the Pattern page manager
	if( theApp.m_pIPageManager
	&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_PatternPropPageManager ) == S_OK )
	{
		pPageManager = (CDllBasePropPageManager *)theApp.m_pIPageManager;
	}
	else
	{
		pPageManager = new CPatternPropPageManager();
	}

	if( pPageManager == NULL )
	{
		return E_FAIL;
	}

	// Show the Pattern properties
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		if( SUCCEEDED ( pIPropSheet->SetPageManager(pPageManager) ) )
		{
			theApp.m_pIPageManager = pPageManager;
			pPageManager->SetObject( this );
			pIPropSheet->SetActivePage( nActiveTab ); 
		}

		pIPropSheet->Show( TRUE );
		pIPropSheet->Release();
	}

	return S_OK;
}
	
@xref <i IDMUSProdPropSheet>, <om IDMUSProdPropSheet.GetActivePage>, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>
------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl  IDMUSProdPropSheet::SetActivePage

HRESULT FAR EXPORT CPropertySheetCtrl::SetActivePage(
	short nIndex		// @parm [in] Page number.  Zero based index.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( (nIndex < 0)
	||  (nIndex >= m_nNbrPages) )
	{
		return E_INVALIDARG;
	}

#ifdef _DEBUG
	CTabCtrl* pTab = GetTabControl();
	ASSERT_VALID(pTab);
#endif

	HWND hwndFocus = ::GetFocus();
	
	SendMessage( PSM_SETCURSEL, nIndex, NULL );

	if( ::IsWindow( hwndFocus )
	&&	hwndFocus != ::GetFocus() )
	{
		::SetFocus( hwndFocus );
	}

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODPROPSHEET::SHOW
========================================================================================
@method HRESULT | IDMUSProdPropSheet | Show | Shows/hides the <o Framework>'s global
	property sheet. 
 
@comm
	<i IDMUSProdPropSheet> provides the means for a <o Component> to hook into the Framework's
	property sheet control, or <o PropSheet> object.  The Framework creates the PropSheet
	object during application launch and it exists throughout the entire DirectMusic Producer
	work session. A pointer to the Framework's IDMUSProdPropSheet interface can be obtained via
	<om IDMUSProdFramework::QueryInterface>.

@rvalue S_OK | Always succeeds.

@ex The following implementation of <om IDMUSProdPropPageObject.OnShowProperties> displays
	properties for a Pattern in a Style: |

HRESULT CDirectMusicPattern::OnShowProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDllBasePropPageManager* pPageManager;
	short nActiveTab = 0;

	// Get the Pattern page manager
	if( theApp.m_pIPageManager
	&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_PatternPropPageManager ) == S_OK )
	{
		pPageManager = (CDllBasePropPageManager *)theApp.m_pIPageManager;
	}
	else
	{
		pPageManager = new CPatternPropPageManager();
	}

	if( pPageManager == NULL )
	{
		return E_FAIL;
	}

	// Show the Pattern properties
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		if( SUCCEEDED ( pIPropSheet->SetPageManager(pPageManager) ) )
		{
			theApp.m_pIPageManager = pPageManager;
			pPageManager->SetObject( this );
			pIPropSheet->SetActivePage( nActiveTab ); 
		}

		pIPropSheet->Show( TRUE );
		pIPropSheet->Release();
	}

	return S_OK;
}
	
@xref <i IDMUSProdPropSheet>, <om IDMUSProdPropSheet.IsShowing>, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl  IDMUSProdPropSheet::Show

HRESULT FAR EXPORT CPropertySheetCtrl::Show(
	BOOL fShow		// @parm [in] TRUE = Show, FALSE = Hide.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	CControlBar* pBar = pMainFrame->GetControlBar( IDD_PROPERTIES );
	if( pBar )
	{
		pMainFrame->ShowControlBar( pBar, fShow, FALSE );
	}

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODPROPSHEET::ISSHOWING
========================================================================================
@method HRESULT | IDMUSProdPropSheet | IsShowing | Determines the visibilty state of the
	<o Framework>'s global property sheet. 
 
@comm
	<i IDMUSProdPropSheet> provides the means for a <o Component> to hook into the Framework's
	property sheet control, or <o PropSheet> object.  The Framework creates the PropSheet
	object during application launch and it exists throughout the entire DirectMusic Producer
	work session. A pointer to the Framework's IDMUSProdPropSheet interface can be obtained via
	<om IDMUSProdFramework::QueryInterface>.

@rvalue S_OK | The property sheet is visible.
@rvalue S_FALSE | The property sheet is hidden.

@ex The following code excerpt makes sure the property sheet is visible before changing its contents: |

	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED ( pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		if( pIPropSheet->IsShowing() == S_OK )
		{
			// Get properties for node
			IDMUSProdPropPageObject* pIPageObject;
			if( SUCCEEDED ( pINode->QueryInterface( IID_IDMUSProdPropPageObject, (void **)&pIPageObject ) ) )
			{
				if( pIPropSheet->IsEqualPageManagerObject(pIPageObject) != S_OK )
				{
					pIPageObject->OnShowProperties();
				}
				pIPageObject->Release();
			}
		}

		pIPropSheet->Release();
	}
	
@xref <i IDMUSProdPropSheet>, <om IDMUSProdPropSheet.Show>, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl  IDMUSProdPropSheet::IsShowing

HRESULT FAR EXPORT CPropertySheetCtrl::IsShowing( void )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	CControlBar* pBar = pMainFrame->GetControlBar( IDD_PROPERTIES );
	if( pBar )
	{
		if( pBar->GetStyle() & WS_VISIBLE )
		{
			return S_OK;
		}
	}

	return S_FALSE;
}


BEGIN_MESSAGE_MAP(CPropertySheetCtrl, CPropertySheet)
	//{{AFX_MSG_MAP(CPropertySheetCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_REFRESH_TITLE, OnRefreshTitle)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl message handlers

BOOL CPropertySheetCtrl::Create( CWnd* pParentWnd, DWORD dwStyle, DWORD dwExStyle ) 
{
	ASSERT( pParentWnd != NULL );
	ASSERT( dwStyle & WS_CHILD );

	if( (pParentWnd == NULL)
	|| !(dwStyle & WS_CHILD) )
	{
		return FALSE;
	}
	
	return CPropertySheet::Create( pParentWnd, dwStyle, dwExStyle );
}


/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl::OnRefreshTitle

LRESULT CPropertySheetCtrl::OnRefreshTitle( WPARAM wParam, LPARAM lParam )
{
	if( m_pIPageManager )
	{
		BSTR bstrTitle;
		BOOL fAddPropertiesText;

		if( SUCCEEDED ( m_pIPageManager->GetPropertySheetTitle( &bstrTitle, &fAddPropertiesText ) ) )
		{
			ChangeTitle( bstrTitle, fAddPropertiesText );
		}
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl::DoModal

int CPropertySheetCtrl::DoModal() 
{
	// This class does not support modal property sheets.
	
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl::OnInitDialog

void ConvertSystemSizeToSansSerifSize(SIZE& size);

BOOL CPropertySheetCtrl::OnInitDialog() 
{
	CPropertySheet::OnInitDialog();

	RECT rect;
	CWnd* pOldParent = GetParent();
	CWnd* pNewParent = pOldParent->GetParent();

	pOldParent->GetWindowRect( &rect );
	SIZE size = { rect.right - rect.left, rect.bottom - rect.top };
	ConvertSystemSizeToSansSerifSize( size );
	SetParent( pNewParent );
	SetWindowPos( NULL, 0, 0, size.cx, size.cy, (SWP_NOZORDER | SWP_SHOWWINDOW) );
	ModifyStyle( 0, DS_CONTROL, 0 );
	ModifyStyleEx( 0, WS_EX_CONTROLPARENT, 0 );
	GetTabControl()->ModifyStyle( 0, WS_TABSTOP, 0 );

	GetTabControl()->GetWindowRect( &rect ); // screen

	size.cx = rect.right - rect.left;
	size.cy = rect.bottom - rect.top;
	ConvertSystemSizeToSansSerifSize( size );
	ScreenToClient( &rect );
	GetTabControl()->SetWindowPos( NULL, rect.left, rect.top, size.cx, size.cy, (SWP_NOZORDER | SWP_SHOWWINDOW) );

	return TRUE;
}
